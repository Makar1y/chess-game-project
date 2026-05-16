#include "Pvp.hpp"
#include "Game.hpp"
#include "Draw.hpp"


namespace {
const int STOCKFISH_ELO_OPTIONS[] = { 1320, 1450, 1600, 1750, 1950, 2200, 2550, 3190 };
const int STOCKFISH_ELO_OPTION_COUNT = sizeof(STOCKFISH_ELO_OPTIONS) / sizeof(STOCKFISH_ELO_OPTIONS[0]);
const float TIME_CONTROL_OPTIONS[] = {60.0f, 180.0f, 300.0f, 600.0f, 900.0f, 1800.0f };
const int TIME_CONTROL_OPTION_COUNT = sizeof(TIME_CONTROL_OPTIONS) / sizeof(TIME_CONTROL_OPTIONS[0]);
}

Game::Game()
    : player1("You", PLAYER_PLAYS_WHITE ? PlayerColor::White : PlayerColor::Black),
      player2("Stockfish", PLAYER_PLAYS_WHITE ? PlayerColor::Black : PlayerColor::White),
      stockfish((StockfishElo)STOCKFISH_ELO_OPTIONS[0]),
      playerTimeLeftSeconds(PLAYER_TIME) {
    isPlayerTurn = player1.getColor() == PlayerColor::White;
}

Game::~Game() {
    pvpConnection.close();
    if (pvpThread.joinable()) {
        pvpThread.join();
    }
}

void Game::resetGameState() {
    board = Board();
    clearSelection();
    clearPossibleMoves();
    moveHistory.clear();
    undoStack = std::stack<Move>();
    whiteCapturedPieces.clear();
    blackCapturedPieces.clear();
    hasLastMove = false;
    winnerName.clear();
    winReason.clear();
    overlayMessage.clear();
    overlayType = OverlayType::None;
    playerTimeLeftSeconds = selectedTimeControlSeconds;
    halfMoves = 0;

    if (gameMode != GameMode::Pvp) {
        stockfish.newGame();
    }
}

int Game::getPvpPort() {
    int port = std::atoi(pvpPortText.c_str());
    if (port <= 0 || port > 65535) return 54000;
    return port;
}

void Game::beginPvpConnection(bool host) {
    if (pvpThread.joinable()) {
        pvpThread.join();
    }

    pvpThreadDone = false;
    pvpThreadSuccess = false;
    pvpHost = host;
    pvpStatusText = host ? "Waiting for guest..." : "Connecting to host...";
    screenState = ScreenState::Connecting;

    int port = getPvpPort();
    std::string ip = pvpIpText;

    if (!host) {
        size_t colon = ip.rfind(':');
        if (colon != std::string::npos && colon + 1 < ip.size()) {
            port = std::atoi(ip.substr(colon + 1).c_str());
            ip = ip.substr(0, colon);
        }
    }

    pvpThread = std::thread([this, host, ip, port]() {
        bool ok = host ? pvpConnection.host(port) : pvpConnection.join(ip, port);
        pvpThreadSuccess = ok;
        pvpThreadDone = true;
    });
}

void Game::handleMainMenu() {
    if (draw.getInput().isLeftMousePressed()) {
        if (draw.getInput().isStartGameClicked()) {
            startGame(playerPlaysWhite, selectedElo, selectedTimeControlSeconds);
        } else if (draw.getInput().isPvpMenuClicked()) {
            pvpStatusText.clear();
            editingIp = false;
            editingPort = false;
            screenState = ScreenState::PvpMenu;
        } else if (draw.getInput().isExitClicked()) {
            exitRequested = true;
            return;
        } else if (draw.getInput().isSelectWhiteClicked()) {
            playerPlaysWhite = true;
        } else if (draw.getInput().isSelectBlackClicked()) {
            playerPlaysWhite = false;
        } else if (draw.getInput().isSelectEloLeftClicked()) {
            selectedElo = getPreviousStockfishElo(selectedElo);
        } else if (draw.getInput().isSelectEloRightClicked()) {
            selectedElo = getNextStockfishElo(selectedElo);
        } else if (draw.getInput().isSelectTimeLeftClicked()) {
            selectedTimeControlSeconds = getPreviousTimeControl(selectedTimeControlSeconds);
        } else if (draw.getInput().isSelectTimeRightClicked()) {
            selectedTimeControlSeconds = getNextTimeControl(selectedTimeControlSeconds);
        }
    }

    draw.mainMenu(playerPlaysWhite, getDifficultyName(selectedElo), selectedTimeControlSeconds);
}

void Game::handlePvpMenu() {
    if (editingIp) draw.getInput().handleTextInput(pvpIpText, 64, false);
    if (editingPort) draw.getInput().handleTextInput(pvpPortText, 5, true);

    if (draw.getInput().isLeftMousePressed()) {
        if (draw.getInput().isIpInputClicked()) {
            editingIp = true;
            editingPort = false;
        } else if (draw.getInput().isPortInputClicked()) {
            editingIp = false;
            editingPort = true;
        } else if (draw.getInput().isHostGameClicked()) {
            editingIp = false;
            editingPort = false;
            beginPvpConnection(true);
        } else if (draw.getInput().isJoinGameClicked()) {
            editingIp = false;
            editingPort = false;
            beginPvpConnection(false);
        } else if (draw.getInput().isBackClicked()) {
            editingIp = false;
            editingPort = false;
            screenState = ScreenState::MainMenu;
        } else {
            editingIp = false;
            editingPort = false;
        }
    }

    if (screenState == ScreenState::PvpMenu) {
        draw.pvpMenu(pvpIpText, pvpPortText, pvpStatusText, editingIp, editingPort);
    }
}

void Game::handleConnectingMenu() {
    if (pvpThreadDone) {
        if (pvpThread.joinable()) {
            pvpThread.join();
        }

        if (pvpThreadSuccess) {
            startPvpGame(&pvpConnection, pvpHost);
            return;
        }

        pvpStatusText = "Connection failed.";
        screenState = ScreenState::PvpMenu;
        return;
    }

    draw.connectingMenu(pvpStatusText);
}

bool Game::applyMove(Move& move, bool sendToOpponent) {
    int fx, fy, tx, ty;
    unpackMove(move, fx, fy, tx, ty);

    Piece* movingPiece = board.getPiece(move.getFromX(), move.getFromY());
    if (movingPiece == nullptr) return false;

    PieceColor moverColor = movingPiece->getColor();
    bool isPawnMove = movingPiece->getType() == PieceType::Pawn;

    if (!validateMove(move) || !isThereNoCheck(move)) {
        return false;
    }

    std::string uci = moveToUci(move);
    bool isEnPassantMove = wasIsEnPassant(move);
    bool isCaptureMove = board.getPiece(move.getToX(), move.getToY()) != nullptr || isEnPassantMove;

    if (isEnPassantMove) {
        removeEnPassantPawn(move);
    }

    halfMoves++;
    if (isCaptureMove || isPawnMove) {
        halfMoves = 0;
    }

    if (isCaptureMove) {
        Piece* captured = board.getPiece(move.getToX(), move.getToY());
        if (captured) {
            if (moverColor == PieceColor::White) whiteCapturedPieces.push_back(captured->getType());
            else blackCapturedPieces.push_back(captured->getType());
        } else if (isEnPassantMove) {
            if (moverColor == PieceColor::White) whiteCapturedPieces.push_back(PieceType::Pawn);
            else blackCapturedPieces.push_back(PieceType::Pawn);
        }
    }

    board.update(move);
    bool isCastleMove = wasItcastle(move);

    if (wasItcastle(move)) {
        moveRook(move);
    }

    moveHistory.push_back(uci);

    if(move.getPromotionPiece() != 'e') {
        promotePawnEngine(move);
    }

    if (wasItPawnPromotio(move)) {
        overlayType = OverlayType::Promotion;
    }

    if (!pvpMode) {
        undoStack.push(std::move(move));
    }

    Move displayMove = uciToMove(uci);
    lastMove = std::move(displayMove);
    hasLastMove = true;

    if (sendToOpponent && pvp != nullptr) {
        pvp->sendMove(uci);
    }

    clearSelection();
    clearPossibleMoves();

    PieceColor opponentColor = moverColor == PieceColor::White ? PieceColor::Black : PieceColor::White;
    bool isCheckMove = checkForCheck(opponentColor);
    checkForGameEnd(opponentColor);

    if (overlayType == OverlayType::None) {
        audio.playMoveEvent(isCaptureMove, isCastleMove, isCheckMove);
    }

    return true;
}

void Game::update() {
    if (pvpMode) {
        receivePvpMessages();
    }

    if (overlayType == OverlayType::None && isPlayerTurn) {
        playerTimeLeftSeconds -= GetFrameTime();
        if (playerTimeLeftSeconds <= 0.0f) {
            playerTimeLeftSeconds = 0.0f;
            winnerName = player2.getName();
            winReason = "Time out";
            overlayType = OverlayType::Results;
            audio.playGameEnd();
            return;
        }
    }

    if (overlayType != OverlayType::None) {
        if (!draw.getInput().isLeftMousePressed()) return;

        if (overlayType == OverlayType::MoveHistory) {
            if (draw.getInput().isBackToGameClicked()) {
                overlayType = OverlayType::None;
            }
        } else if (overlayType == OverlayType::Info) {
            overlayMessage.clear();
            overlayType = OverlayType::None;
        } else if (overlayType == OverlayType::Results) {
            if (draw.getInput().isExitToMenuClicked()) {
                goToMainMenu();
            } else if (draw.getInput().isNewGameClicked()) {
                startNewGame();
            }
        } else if (draw.getInput().isOverlayYesClicked()) {
            if (overlayType == OverlayType::Resign) {
                if (pvpMode && pvp != nullptr) {
                    pvp->sendLine("RESIGN");
                }
                winnerName = player2.getName();
                winReason = "Resignation";
                overlayType = OverlayType::Results;
                audio.playGameEnd();
            } else if (overlayType == OverlayType::Draw) {
                if (pvpMode && pvp != nullptr) {
                    pvp->sendLine("DRAW_OFFER");
                    overlayMessage = "Draw offer sent.";
                } else {
                    overlayMessage = "Draw offer rejected automatically.";
                }
                overlayType = OverlayType::Info;
            } else if (overlayType == OverlayType::DrawOfferReceived) {
                if (pvpMode && pvp != nullptr) {
                    pvp->sendLine("DRAW_ACCEPTED");
                }
                winnerName = "Draw";
                winReason = "Draw agreed";
                overlayType = OverlayType::Results;
                audio.playGameEnd();
            }
        } else if (draw.getInput().isOverlayNoClicked()) {
            if (overlayType == OverlayType::DrawOfferReceived && pvpMode && pvp != nullptr) {
                pvp->sendLine("DRAW_REJECTED");
                overlayMessage = "Draw offer rejected.";
                overlayType = OverlayType::Info;
            } else {
                overlayType = OverlayType::None;
            }
        } else if (overlayType == OverlayType::Promotion) {
            PieceType choice = promotionPieceSelected();
            if(choice != PieceType::None) {
                promotePawn(choice);
                overlayType = OverlayType::None;
                return;
            }
            if(draw.getInput().isCancelPromotionClicked() || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                overlayType = OverlayType::None;
                undoLastMove();
            }
            
        }

        return;
    }

    if (draw.getInput().isLeftMousePressed()) {
        if (draw.getInput().isResignClicked()) {
            resign();
            return;
        }

        if (!pvpMode && draw.getInput().isUndoClicked()) {
            undoLastMove();
            return;
        }

        if (draw.getInput().isOfferDrawClicked()) {
            offerdaw();
            return;
        }

        if (draw.getInput().isShowResultsClicked()) {
            showMoveHistory();
            return;
        }
    }

    if (!isPlayerTurn) return;
    if (!draw.getInput().isLeftMousePressed()) return;

    int x = -1;
    int y = -1;

    if (!draw.getInput().getClickedBoardCell(x, y, playerPlaysWhite)) return;

    Piece* clickedPiece = board.getPiece(x, y);
    PieceColor playerPieceColor = playerPlaysWhite ? PieceColor::White : PieceColor::Black;

    if (!pieceSelected) {
        if (clickedPiece != nullptr && clickedPiece->getColor() == playerPieceColor) {
            pieceSelected = true;
            selectedX = x;
            selectedY = y;
            getPossibleMoves(x, y);
        }
        return;
    }

    if (x == selectedX && y == selectedY) {
        clearSelection();
        clearPossibleMoves();
        return;
    }

    Move newMove = convertToMove(selectedX, selectedY, x, y);

    if (applyMove(newMove, pvpMode)) {
        if (overlayType == OverlayType::None) {
            isPlayerTurn = false;
        }
    } else if (board.getPiece(x, y) != nullptr && board.getPiece(x, y)->getColor() == playerPieceColor) {
        selectedX = x;
        selectedY = y;
        getPossibleMoves(x, y);
    }
}

void Game::getPossibleMoves(int x, int y) {
    clearPossibleMoves();
    for(int i = 0; i < 8; ++i) {
        for(int j = 0; j < 8; ++j) {
            if(x == i && y == j) continue;
            Move newMove = convertToMove(x, y, i, j);
            if(validateMove(newMove) && isThereNoCheck(newMove)) {
                if(isCapture(x, y ,i ,j)) {
                    possibleCaptures.insert(std::make_pair(i, j));
                } else {
                    possibleMoves.insert(std::make_pair(i, j));
                }
            }
        }
    }
}

Move Game::convertToMove(int fx, int fy, int tx, int ty) {
    Move newMove;
    newMove.setFromX(fx);
    newMove.setFromY(fy);
    newMove.setToX(tx);
    newMove.setToY(ty);
    return newMove;
}

void Game::unpackMove(Move& move, int& fx, int& fy, int& tx, int& ty) {
    fx = move.getFromX();
    fy = move.getFromY();
    tx = move.getToX();
    ty = move.getToY();
}

void Game::clearPossibleMoves() {
    possibleMoves.clear();
    possibleCaptures.clear();
}

bool Game::isCapture(int fx, int fy, int tx, int ty) {
    Piece *piece = board.getPiece(fx, fy);
    Piece *attackedPiece = board.getPiece(tx, ty);

    if(piece == nullptr || attackedPiece == nullptr) return false;
    if(piece->getColor() != attackedPiece->getColor()) return true;

    return false;
}

bool Game::checkForCheck(PieceColor color) {
    int x = -1, y = -1;
    for(int i = 0; i < 8; ++i) {
        for(int j = 0; j < 8; ++j) {
            Piece *piece = board.getPiece(i, j);
            if(piece == nullptr) continue;
            if(piece->getType() == PieceType::King && piece->getColor() == color) {
                x = i;
                y = j;
            }
        }
    }
    if(x == -1 || y == -1) return false;
    for(int i = 0; i < 8; ++i) {
        for(int j = 0; j < 8; ++j) {
            Piece *piece = board.getPiece(i, j);
            if(piece == nullptr) continue;
            Move take = convertToMove(i, j, x, y);
            if(validateMove(take) && isCapture(i, j, x, y)) {
                return true;
            }
        }
    }

    return false;
}

bool Game::isThereNoCheck(Move& move) {
    int fx, fy, tx, ty;
    unpackMove(move, fx, fy, tx, ty);
    board.update(move);
    bool flag;
    if(checkForCheck(board.getPiece(tx, ty)->getColor())) {
        flag = false;
    } else {
        flag = true;
    }
    board.undo(move);
    return flag;
}

bool Game::hasAnyLegalMove(PieceColor color) {
    for (int fx = 0; fx < 8; ++fx) {
        for (int fy = 0; fy < 8; ++fy) {
            Piece* piece = board.getPiece(fx, fy);
            if (piece == nullptr || piece->getColor() != color) continue;

            for (int tx = 0; tx < 8; ++tx) {
                for (int ty = 0; ty < 8; ++ty) {
                    if (fx == tx && fy == ty) continue;
                    Move move = convertToMove(fx, fy, tx, ty);
                    if (validateMove(move) && isThereNoCheck(move)) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void Game::checkForGameEnd(PieceColor color) {
    if(halfMoves == 100) {
        winnerName = "Draw";
        winReason = "50-move rule";
    } else {
        if (hasAnyLegalMove(color)) return;

        if (checkForCheck(color)) {
            winnerName = (color == (playerPlaysWhite ? PieceColor::White : PieceColor::Black)) ? player2.getName() : player1.getName();
            winReason = "Checkmate";
        } else {
            winnerName = "Draw";
            winReason = "Stalemate";
        }
    }

    overlayType = OverlayType::Results;
    isPlayerTurn = true;
    audio.playGameEnd();
}

void Game::makeStockfishMove() {
    if (overlayType != OverlayType::None) return;
    if (isPlayerTurn) return;

    std::string bestMove = stockfish.move(moveHistory);

    if (bestMove.empty() || bestMove == "(none)" || bestMove == "0000") {
        PieceColor stockfishPieceColor = playerPlaysWhite ? PieceColor::Black : PieceColor::White;
        checkForGameEnd(stockfishPieceColor);
        isPlayerTurn = true;
        return;
    }

    Move move = uciToMove(bestMove);
    if (applyMove(move, false) && overlayType == OverlayType::None) {
        isPlayerTurn = true;
    }
}

void Game::makePvpMove(const std::string& uci) {
    if (uci.size() < 4) return;

    Move move = uciToMove(uci);
    if (applyMove(move, false) && overlayType == OverlayType::None) {
        isPlayerTurn = true;
    }
}

void Game::receivePvpMessages() {
    if (!pvpMode || pvp == nullptr) return;

    while (true) {
        std::string message = pvp->receiveMove();
        if (message.empty()) break;
        handlePvpMessage(message);
    }
}

void Game::handlePvpMessage(const std::string& message) {
    if (message.rfind("MOVE ", 0) == 0) {
        makePvpMove(message.substr(5));
        return;
    }

    if (message.size() == 4) {
        makePvpMove(message);
        return;
    }

    if (message == "RESIGN") {
        winnerName = player1.getName();
        winReason = "Opponent resigned";
        overlayType = OverlayType::Results;
        isPlayerTurn = true;
        audio.playGameEnd();
        return;
    }

    if (message == "NEW_GAME") {
        startPvpGame(pvp, pvpHost);
        return;
    }

    if (message == "DRAW_OFFER") {
        overlayType = OverlayType::DrawOfferReceived;
        return;
    }

    if (message == "DRAW_ACCEPTED") {
        winnerName = "Draw";
        winReason = "Draw agreed";
        overlayType = OverlayType::Results;
        isPlayerTurn = true;
        audio.playGameEnd();
        return;
    }

    if (message == "DRAW_REJECTED") {
        overlayMessage = "Draw offer rejected.";
        overlayType = OverlayType::Info;
        return;
    }
}

void Game::startGame(bool playerPlaysWhite, int stockfishElo, float timeControlSeconds) {
    if (pvpThread.joinable()) {
        pvpConnection.close();
        pvpThread.join();
    }

    pvpConnection.close();
    pvp = nullptr;
    pvpMode = false;
    gameMode = GameMode::Stockfish;

    this->playerPlaysWhite = playerPlaysWhite;
    selectedElo = stockfishElo;
    selectedTimeControlSeconds = timeControlSeconds;
    stockfish.setElo((StockfishElo)stockfishElo);
    player1.setName("You");
    player2.setName("Stockfish");
    player1.setColor(playerPlaysWhite ? PlayerColor::White : PlayerColor::Black);
    player2.setColor(playerPlaysWhite ? PlayerColor::Black : PlayerColor::White);
    isPlayerTurn = playerPlaysWhite;
    resetGameState();
    audio.playGameStart();
    screenState = ScreenState::InGame;
}

void Game::startPvpGame(Pvp* pvp, bool host) {
    this->pvp = pvp;
    pvpMode = true;
    gameMode = GameMode::Pvp;
    pvpHost = host;
    playerPlaysWhite = host;
    player1.setName("You");
    player2.setName("Guest");
    player1.setColor(host ? PlayerColor::White : PlayerColor::Black);
    player2.setColor(host ? PlayerColor::Black : PlayerColor::White);
    isPlayerTurn = host;
    resetGameState();
    audio.playGameStart();
    screenState = ScreenState::InGame;
}

void Game::mainMenu() {
    draw.initWindow();
    audio.load();

    while (!exitRequested && !draw.shouldClose()) {
        if (screenState == ScreenState::MainMenu) {
            handleMainMenu();
            continue;
        }

        if (screenState == ScreenState::PvpMenu) {
            handlePvpMenu();
            continue;
        }

        if (screenState == ScreenState::Connecting) {
            handleConnectingMenu();
            continue;
        }

        update();
        if (screenState != ScreenState::InGame) continue;

        draw.render(board, pieceSelected, selectedX, selectedY, possibleMoves, possibleCaptures,
            &lastMove, hasLastMove, overlayType, player2.getName(), playerPlaysWhite,
            playerTimeLeftSeconds, moveHistory, winnerName, winReason, overlayMessage, pvpMode,
            whiteCapturedPieces, blackCapturedPieces);

        if (screenState == ScreenState::InGame && !isPlayerTurn && !pvpMode) {
            std::this_thread::sleep_for(std::chrono::milliseconds(STOCKFISH_MOVE_TIME_MS));
            makeStockfishMove();
        }
    }

    pvpConnection.close();
    if (pvpThread.joinable()) {
        pvpThread.join();
    }

    audio.unload();
    draw.closeWindow();
}

int Game::getNextStockfishElo(int currentElo) {
    for (int i = 0; i < STOCKFISH_ELO_OPTION_COUNT; i++) {
        if (STOCKFISH_ELO_OPTIONS[i] == currentElo) {
            return STOCKFISH_ELO_OPTIONS[(i + 1) % STOCKFISH_ELO_OPTION_COUNT];
        }
    }

    return STOCKFISH_ELO_OPTIONS[0];
}

int Game::getPreviousStockfishElo(int currentElo) {
    for (int i = 0; i < STOCKFISH_ELO_OPTION_COUNT; i++) {
        if (STOCKFISH_ELO_OPTIONS[i] == currentElo) {
            int prevIndex = (i - 1 + STOCKFISH_ELO_OPTION_COUNT) % STOCKFISH_ELO_OPTION_COUNT;
            return STOCKFISH_ELO_OPTIONS[prevIndex];
        }
    }

    return STOCKFISH_ELO_OPTIONS[0];
}

float Game::getNextTimeControl(float currentTimeControlSeconds) {
    for (int i = 0; i < TIME_CONTROL_OPTION_COUNT; i++) {
        if (TIME_CONTROL_OPTIONS[i] == currentTimeControlSeconds) {
            return TIME_CONTROL_OPTIONS[(i + 1) % TIME_CONTROL_OPTION_COUNT];
        }
    }

    return TIME_CONTROL_OPTIONS[0];
}

float Game::getPreviousTimeControl(float currentTimeControlSeconds) {
    for (int i = 0; i < TIME_CONTROL_OPTION_COUNT; i++) {
        if (TIME_CONTROL_OPTIONS[i] == currentTimeControlSeconds) {
            int prevIndex = (i - 1 + TIME_CONTROL_OPTION_COUNT) % TIME_CONTROL_OPTION_COUNT;
            return TIME_CONTROL_OPTIONS[prevIndex];
        }
    }

    return TIME_CONTROL_OPTIONS[0];
}

std::string Game::getDifficultyName(int elo) {
    if (elo <= 1320) return "Beginner";
    if (elo <= 1450) return "Casual";
    if (elo <= 1600) return "Intermediate";
    if (elo <= 1750) return "Advanced";
    if (elo <= 1950) return "Expert";
    if (elo <= 2200) return "Master";
    if (elo <= 2550) return "Grandmaster";
    return "Impossible";
}

std::string Game::moveToUci(Move& move) {
    std::string uci = "0000";
    uci[0] = (char)('a' + move.getFromX());
    uci[1] = (char)('8' - move.getFromY());
    uci[2] = (char)('a' + move.getToX());
    uci[3] = (char)('8' - move.getToY());
    char promotedPiece = move.getPromotionPiece();
    if(promotedPiece != 'e') {
        uci += promotedPiece;
    }
    return uci;
}

Move Game::uciToMove(const std::string& uciMove) {
    Move move;
    move.setFromX(uciMove[0] - 'a');
    move.setFromY('8' - uciMove[1]);
    move.setToX(uciMove[2] - 'a');
    move.setToY('8' - uciMove[3]);
    if(uciMove.size() > 4)
        move.setPromotionPiece(uciMove[4]);
    return move;
}

void Game::clearSelection() {
    pieceSelected = false;
    selectedX = -1;
    selectedY = -1;
}

void Game::showResults() {
    winnerName = "Unknown";
    winReason = "Manual Results View";
    draw.resetHistoryScroll();
    overlayType = OverlayType::Results;
}

void Game::showMoveHistory() {
    draw.resetHistoryScroll();
    overlayType = OverlayType::MoveHistory;
}

void Game::goToMainMenu() {
    if (pvpMode) {
        pvpConnection.close();
        pvp = nullptr;
        pvpMode = false;
        gameMode = GameMode::Stockfish;
        player2.setName("Stockfish");
    }

    resetGameState();
    isPlayerTurn = player1.getColor() == PlayerColor::White;
    screenState = ScreenState::MainMenu;
}

void Game::startNewGame() {
    if (pvpMode && pvp != nullptr) {
        pvp->sendLine("NEW_GAME");
        startPvpGame(pvp, pvpHost);
        return;
    }

    startGame(playerPlaysWhite, selectedElo, selectedTimeControlSeconds);
}

void Game::resign() {
    overlayType = OverlayType::Resign;
}

void Game::offerDraw() {
    overlayType = OverlayType::Draw;
}

void Game::offerdaw() {
    offerDraw();
}

bool Game::validateMove(Move& move) {
    int fx, fy, tx, ty;
    unpackMove(move, fx, fy, tx, ty);

    Piece *piece = board.getPiece(fx, fy);
    if (piece == nullptr) return false;
    
    switch (piece->getType())
    {
    case PieceType::Pawn:
        return isValidPawnMove(fx, fy, tx, ty);
    case PieceType::Rook:
        return isValidRockMove(fx, fy, tx, ty);
    case PieceType::Bishop:
        return isValidBishopMove(fx, fy, tx, ty);
    case PieceType::Queen:
        return isValidQueenMove(fx, fy, tx, ty);
    case PieceType::Knight:
        return isValidKnightMove(fx, fy, tx, ty);
    case PieceType::King:
        return isValidKingMove(fx, fy, tx, ty);
    default:
        break;
    }
    return true;
}

bool Game::isValidPawnMove(int fx, int fy, int tx, int ty) {
    Piece *piece = board.getPiece(fx, fy);
    PieceColor color = piece->getColor();

    if(areSameColorPieces(fx, fy, tx, ty)) {
        return false;
    }

    int direction = (color == PieceColor::White)? -1:1;

    if(fx == tx) {
        // One move forward
        if(fy + direction == ty && board.getPiece(tx, ty) == nullptr) {
            return true;
        }
        // Two moves forward
        if(!piece->getHasMoved() && fy + 2 * direction == ty &&
            board.getPiece(tx, fy + direction) == nullptr && board.getPiece(tx, ty) == nullptr) {
                return true;
            }
    }

    // Capture
    if(abs(fx - tx) == 1 && fy + direction == ty && board.getPiece(tx, ty) != nullptr) {
        return true;
    }

    // en passsant
    if (abs(tx - fx) != 1 || ty != fy + direction) return false;
    if(!hasLastMove) return false;

    Piece* enemyPiece = board.getPiece(lastMove.getToX(), lastMove.getToY());
    if(enemyPiece == nullptr) return false;
    if(enemyPiece->getType() != PieceType::Pawn) return false;
    if(enemyPiece->getColor() == piece->getColor()) return false;
    if(abs(lastMove.getFromY() - lastMove.getToY()) != 2) return false;
    if(lastMove.getToY() != fy) return false;
    if(lastMove.getToX() != tx) return false;

    return true;
}

bool Game::isValidRockMove(int fx, int fy, int tx, int ty) {
    if(areSameColorPieces(fx, fy, tx, ty)) {
        return false;
    }

    if((fx == tx || fy == ty) && isPathClear(fx, fy, tx, ty)) {
        return true;
    }

    return false;
}

bool Game::isValidBishopMove(int fx, int fy, int tx, int ty) {
    if(areSameColorPieces(fx, fy, tx, ty)) {
        return false;
    }

    int difX = abs(fx - tx);
    int difY = abs(fy - ty);

    if(difX == difY && isPathClear(fx, fy, tx, ty)) {
        return true;
    }

    return false;
}

bool Game::isValidQueenMove(int fx, int fy, int tx, int ty) {
    if(areSameColorPieces(fx, fy, tx, ty)) {
        return false;
    }

    int difX = abs(fx - tx);
    int difY = abs(fy - ty);

    if((difX == difY || (fx == tx || fy == ty))  && isPathClear(fx, fy, tx, ty)) {
        return true;
    }

    return false;
}

bool Game::isValidKnightMove(int fx, int fy, int tx, int ty) {
    if(areSameColorPieces(fx, fy, tx, ty)) {
        return false;
    }

    int difX = abs(fx - tx);
    int difY = abs(fy - ty);

    if(difX != 0 && difY != 0 && difY + difX == 3) {
      return true;
    }

    return false;
}

bool Game::isValidKingMove(int fx, int fy, int tx, int ty) {
    if(areSameColorPieces(fx, fy, tx, ty)) {
        return false;
    }

    int difX = abs(fx - tx);
    int difY = abs(fy - ty);
    
    if(difX <= 1 && difY <= 1) {
        return true;
    }

    // Checking castling
    Piece* king = board.getPiece(fx, fy);

    if(king->getHasMoved()) return false;

    if(fy != ty) return false;

    if(abs(fx - tx) != 2) return false;

    int rookX = (tx > fx) ? 7 : 0;
    Piece* rook = board.getPiece(rookX, ty);

    if(rook == nullptr) return false;
    if(rook->getType() != PieceType::Rook) return false;
    if(rook->getHasMoved()) return false;
    if(rook->getColor() != king->getColor()) return false;

    int direction = (tx > fx) ? 1 : -1;
    
    for(int x = fx + direction; x != rookX; x += direction) {
        if(board.getPiece(x, ty) != nullptr) return false;
    }

    for(int i = 0; i <= 2; ++i) {
        int x = fx + i * direction;

        for(int j = 0; j < 8; j++) {
            for(int k = 0; k < 8; k++) {
                Piece* piece = board.getPiece(j, k);
                if(piece == nullptr || piece->getColor() == king->getColor()) continue;
                Move take = convertToMove(j, k, x, fy);
                if(validateMove(take)) return false;
            }
        }
    }
    return true;
}


// Checks if path horizontally and diagonally is clear
bool Game::isPathClear(int fx, int fy, int tx, int ty) {
    int dx = (tx > fx) - (tx < fx);
    int dy = (ty > fy) - (ty < fy);

    int cx = fx + dx;
    int cy = fy + dy;
    while(cx != tx || cy != ty) {
        if(board.getPiece(cx, cy) != nullptr) return false;
        cx += dx;
        cy += dy;
    }

    return true;
}

bool Game::areSameColorPieces(int fx, int fy, int tx, int ty) {
    Piece *piece = board.getPiece(fx, fy);
    PieceColor color = piece->getColor();
    Piece *attackedPiece = board.getPiece(tx, ty);

    if(attackedPiece == nullptr) {
        return false;
    }
    if(piece->getColor() == attackedPiece->getColor()) {
        return true;
    }
    return false;
}

bool Game::wasItcastle(Move& move) {
    int fx, fy, tx, ty;
    unpackMove(move, fx, fy, tx, ty);
    if(board.getPiece(tx, ty)->getType() == PieceType::King && abs(fx - tx) == 2) return true;
    return false;
}

void Game::moveRook(Move& move) {
    int fx, fy, tx, ty;
    unpackMove(move, fx, fy, tx, ty);
    int rookFromX = (fx > tx) ? 0 : 7;
    int rookToX = (fx > tx) ? 3 : 5;
    Move moveRook = convertToMove(rookFromX, fy, rookToX, ty);
    board.update(moveRook);
}

bool Game::wasIsEnPassant(Move& move) {
    int fx, fy, tx, ty;
    unpackMove(move, fx, fy, tx, ty);
    if(board.getPiece(fx, fy)->getType() == PieceType::Pawn && board.getPiece(tx, ty) == nullptr && fx != tx){
        return true;
    }
    return false;
}

void Game::removeEnPassantPawn(Move& move) {
    int fx, fy, tx, ty;
    unpackMove(move, fx, fy, tx, ty);
    if(fx < tx) {
        std::unique_ptr<Piece> pawn = board.getPieceOwnership(fx + 1, fy);
        pawn.reset();
    } else {
        std::unique_ptr<Piece> pawn = board.getPieceOwnership(fx - 1, fy);
        pawn.reset();
    }
}

bool Game::wasItPawnPromotio(Move& move) {
    int fx, fy, tx, ty;
    unpackMove(move, fx, fy, tx, ty);
    if(board.getPiece(tx, ty)->getType() == PieceType::Pawn && (ty == 0 || ty == 7)) return true;
    return false;
}
void Game::undoLastMove() {
    if (undoStack.empty()) return;
    Move last = std::move(undoStack.top());
    undoStack.pop();

    board.undo(last);

    Piece* restored = board.getPiece(last.getToX(), last.getToY());
    if (restored) {
        if (restored->getColor() == PieceColor::Black) {
            if (!whiteCapturedPieces.empty()) whiteCapturedPieces.pop_back();
        } else {
            if (!blackCapturedPieces.empty()) blackCapturedPieces.pop_back();
        }
    }

    if (!moveHistory.empty()) {
        moveHistory.pop_back();
    }

    hasLastMove = false;
}

void Game::promotePawn(PieceType choice) {
    std::string lastmove = moveHistory.back().substr(2, 2);
    int x = lastmove[0] - 'a';
    int y = '8' - lastmove[1];
    switch (choice)
    {
    case PieceType::Queen:
        moveHistory.back() += 'q';
        break;
    case PieceType::Rook:
        moveHistory.back() += 'r';
        break;
    case PieceType::Knight:
        moveHistory.back() += 'n';
        break;
    case PieceType::Bishop:
        moveHistory.back() += 'b';
        break;
    }
    isPlayerTurn = false;
    board.getPiece(x, y)->setType(choice);
}

bool Game::wasPawnMove(int x, int y) {
    if(board.getPiece(x, y) == nullptr) return false;
    return (board.getPiece(x, y)->getType() == PieceType::Pawn);
}

PieceType Game::promotionPieceSelected() {
    if(draw.getInput().isPromoteQueenSelectionClicked()) {
        return PieceType::Queen;
    }
    if(draw.getInput().isPromoteKnightSelectionClicked()) {
        return PieceType::Knight;
    }
    if(draw.getInput().isPromoteRookSelectionClicked()) {
        return PieceType::Rook;
    }
    if(draw.getInput().isPromoteBishopSelectionClicked()) {
        return PieceType::Bishop;
    }
    return PieceType::None;
}

void Game::promotePawnEngine(Move& move) {
    PieceType choice;
    switch (move.getPromotionPiece())
    {
    case 'q':
        choice = PieceType::Queen;
        break;
    case 'n':
        choice = PieceType::Knight;
        break;
    case 'r':
        choice = PieceType::Rook;
        break;
    case 'b':
        choice = PieceType::Bishop;
        break;
    }
    int x = move.getToX();
    int y = move.getToY();
    board.getPiece(x, y)->setType(choice);
}