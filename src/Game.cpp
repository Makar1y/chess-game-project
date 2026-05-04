#include "Game.hpp"
#include "Draw.hpp"

namespace {
const int STOCKFISH_ELO_OPTIONS[] = { 1320, 1450, 1600, 1750, 1950, 2200, 2550, 3190 };
const int STOCKFISH_ELO_OPTION_COUNT = sizeof(STOCKFISH_ELO_OPTIONS) / sizeof(STOCKFISH_ELO_OPTIONS[0]);
const float TIME_CONTROL_OPTIONS[] = {60.0f, 180.0f, 300.0f, 600.0f, 900.0f, 1800.0f };
const int TIME_CONTROL_OPTION_COUNT = sizeof(TIME_CONTROL_OPTIONS) / sizeof(TIME_CONTROL_OPTIONS[0]);
}

Game::Game()
    : player1("Player 1", PLAYER_PLAYS_WHITE ? PlayerColor::White : PlayerColor::Black),
      player2("Stockfish", PLAYER_PLAYS_WHITE ? PlayerColor::Black : PlayerColor::White),
      stockfish((StockfishElo)STOCKFISH_ELO_OPTIONS[0]),
      playerTimeLeftSeconds(PLAYER_TIME) {
    isPlayerTurn = player1.getColor() == PlayerColor::White;
}

void Game::resetGameState() {
    board = Board();
    clearSelection();
    clearPossibleMoves();
    moveHistory.clear();
    undoStack = std::stack<Move>();
    hasLastMove = false;
    winnerName.clear();
    winReason.clear();
    overlayMessage.clear();
    overlayType = OverlayType::None;
    playerTimeLeftSeconds = selectedTimeControlSeconds;
    stockfish.newGame();
}

void Game::update() {
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
                winnerName = player2.getName();
                winReason = "Resignation";
                overlayType = OverlayType::Results;
                audio.playGameEnd();
            } else if (overlayType == OverlayType::Draw) {
                overlayMessage = "Draw offer rejected automatically.";
                overlayType = OverlayType::Info;
            }
        } else if (draw.getInput().isOverlayNoClicked()) {
            overlayType = OverlayType::None;
        }

        return;
    }

    if (!isPlayerTurn) return;

    if (IsKeyPressed(KEY_U)) {
        undoLastMove();
        return;
    }
    if (!draw.getInput().isLeftMousePressed()) return;

    if (draw.getInput().isResignClicked()) {
        resign();
        return;
    }
    if (draw.getInput().isUndoClicked()) {
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

    if (overlayType != OverlayType::None) {
        if (draw.getInput().isExitToMenuClicked()) {
            goToMainMenu();
            return;
        }

        if (draw.getInput().isNewGameClicked()) {
            startNewGame();
            return;
        }
    }

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

    if (validateMove(newMove) && isThereNoCheck(newMove)) {
        bool isEnPassantMove = wasIsEnPassant(newMove);
        bool isCaptureMove = board.getPiece(x, y) != nullptr || isEnPassantMove;
        if (isEnPassantMove) {
            removeEnPassantPawn(newMove);
        }
        halfMoves++;
        if(isCaptureMove || wasPawnMove(x, y)) halfMoves = 0;
        board.update(newMove);
        bool isCastleMove = wasItcastle(newMove);

        undoStack.push(std::move(newMove));
        moveHistory.push_back(moveToUci(newMove));

        if (wasItPawnPromotio(newMove)) {
            promotePawn(newMove);
        }

        if (wasItcastle(newMove)) {
            moveRook(newMove);
        }

        lastMove = std::move(newMove);
        hasLastMove = true;
        clearSelection();
        clearPossibleMoves();

        PieceColor stockfishPieceColor = playerPlaysWhite ? PieceColor::Black : PieceColor::White;
        bool isCheckMove = checkForCheck(stockfishPieceColor);
        checkForGameEnd(stockfishPieceColor);

        if (overlayType == OverlayType::None) {
            audio.playMoveEvent(isCaptureMove, isCastleMove, isCheckMove);
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
    bool isEnPassantMove = wasIsEnPassant(move);
    bool isCaptureMove = board.getPiece(move.getToX(), move.getToY()) != nullptr || isEnPassantMove;
    if (isEnPassantMove) {
        removeEnPassantPawn(move);
    }
    halfMoves++;
    if(isCaptureMove || wasPawnMove(move.getToX(), move.getToY())) halfMoves = 0;
    board.update(move);
    bool isCastleMove = wasItcastle(move);
    moveHistory.push_back(moveToUci(move));
    undoStack.push(std::move(move));

    if (wasItPawnPromotio(move)) {
        promotePawn(move);
    }

    if (wasItcastle(move)) {
        moveRook(move);
    }

    lastMove = std::move(move);
    hasLastMove = true;

    PieceColor playerPieceColor = playerPlaysWhite ? PieceColor::White : PieceColor::Black;
    bool isCheckMove = checkForCheck(playerPieceColor);
    checkForGameEnd(playerPieceColor);

    if (overlayType == OverlayType::None) {
        audio.playMoveEvent(isCaptureMove, isCastleMove, isCheckMove);
        isPlayerTurn = true;
    }
}


void Game::startGame(bool playerPlaysWhite, int stockfishElo, float timeControlSeconds) {
    this->playerPlaysWhite = playerPlaysWhite;
    selectedElo = stockfishElo;
    selectedTimeControlSeconds = timeControlSeconds;
    stockfish.setElo((StockfishElo)stockfishElo);
    player1.setColor(playerPlaysWhite ? PlayerColor::White : PlayerColor::Black);
    player2.setColor(playerPlaysWhite ? PlayerColor::Black : PlayerColor::White);
    isPlayerTurn = playerPlaysWhite;
    resetGameState();
    audio.playGameStart();
    screenState = ScreenState::InGame;
}

void Game::mainMenu() {
    draw.initWindow();
    audio.load();

    while (!draw.shouldClose()) {
        if (screenState == ScreenState::MainMenu) {
            if (draw.getInput().isLeftMousePressed()) {
                if (draw.getInput().isStartGameClicked()) {
                    startGame(playerPlaysWhite, selectedElo, selectedTimeControlSeconds);
                } else if (draw.getInput().isExitClicked()) {
                    break;
                } else if (draw.getInput().isSelectWhiteClicked()) {
                    playerPlaysWhite = true;
                } else if (draw.getInput().isSelectBlackClicked()) {
                    playerPlaysWhite = false;
                } else if (draw.getInput().isSelectEloLeftClicked()) {
                    selectedElo = getPreviousStockfishElo(selectedElo);
                } else if (draw.getInput().isSelectEloRightClicked()) {
                    selectedElo = getNextStockfishElo(selectedElo);
                } else if (draw.getInput().isSelectEloClicked()) {
                    selectedElo = getNextStockfishElo(selectedElo);
                } else if (draw.getInput().isSelectTimeLeftClicked()) {
                    selectedTimeControlSeconds = getPreviousTimeControl(selectedTimeControlSeconds);
                } else if (draw.getInput().isSelectTimeRightClicked()) {
                    selectedTimeControlSeconds = getNextTimeControl(selectedTimeControlSeconds);
                } else if (draw.getInput().isSelectTimeClicked()) {
                    selectedTimeControlSeconds = getNextTimeControl(selectedTimeControlSeconds);
                }
            }

            draw.mainMenu(playerPlaysWhite, selectedElo, selectedTimeControlSeconds);
            continue;
        }

        update();
        draw.render(board, pieceSelected, selectedX, selectedY, possibleMoves, possibleCaptures, &lastMove, hasLastMove, overlayType, player2.getName(), playerPlaysWhite, playerTimeLeftSeconds, moveHistory, winnerName, winReason, overlayMessage);

        if (screenState == ScreenState::InGame && !isPlayerTurn) {
            std::this_thread::sleep_for(std::chrono::milliseconds(STOCKFISH_MOVE_TIME_MS));
            makeStockfishMove();
        }
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

std::string Game::moveToUci(Move& move) {
    std::string uci = "0000";
    uci[0] = (char)('a' + move.getFromX());
    uci[1] = (char)('8' - move.getFromY());
    uci[2] = (char)('a' + move.getToX());
    uci[3] = (char)('8' - move.getToY());
    return uci;
}

Move Game::uciToMove(const std::string& uciMove) {
    Move move;
    move.setFromX(uciMove[0] - 'a');
    move.setFromY('8' - uciMove[1]);
    move.setToX(uciMove[2] - 'a');
    move.setToY('8' - uciMove[3]);
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
    overlayType = OverlayType::Results;
}

void Game::showMoveHistory() {
    overlayType = OverlayType::MoveHistory;
}

void Game::goToMainMenu() {
    resetGameState();
    isPlayerTurn = player1.getColor() == PlayerColor::White;
    screenState = ScreenState::MainMenu;
}

void Game::startNewGame() {
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

    if (!moveHistory.empty()) {
        moveHistory.pop_back();
    }

    hasLastMove = false;
}

void Game::promotePawn(Move& move) {
    int fx, fy, tx, ty;
    unpackMove(move, fx, fy, tx, ty);
    //TODO overlay for chossing pawn promotion
    PieceType choise = PieceType::Queen;

    board.getPiece(tx, ty)->setType(choise);
}

bool Game::wasPawnMove(int x, int y) {
    if(board.getPiece(x, y) == nullptr) return false;
    return (board.getPiece(x, y)->getType() == PieceType::Pawn);
}