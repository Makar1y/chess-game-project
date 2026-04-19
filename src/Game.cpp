#include "Game.hpp"
#include "Draw.hpp"

namespace {
const int STOCKFISH_ELO_OPTIONS[] = { 1320, 1450, 1600, 1750, 1950, 2200, 2550, 3190 };
const int STOCKFISH_ELO_OPTION_COUNT = sizeof(STOCKFISH_ELO_OPTIONS) / sizeof(STOCKFISH_ELO_OPTIONS[0]);
}

Game::Game()
    : player1("Player 1", PLAYER_PLAYS_WHITE ? PlayerColor::White : PlayerColor::Black),
      player2("Stockfish", PLAYER_PLAYS_WHITE ? PlayerColor::Black : PlayerColor::White),
      stockfish((StockfishElo)STOCKFISH_ELO_OPTIONS[0]) {
    isPlayerTurn = player1.getColor() == PlayerColor::White;
}

void Game::update() {
    if (!isPlayerTurn) return;
    if (!draw.getInput().isLeftMousePressed()) return;

    if (overlayType != OverlayType::None) {
        if (draw.getInput().isOverlayButtonClicked()) {
            if (overlayType == OverlayType::Resign || overlayType == OverlayType::Draw) {
                board = Board();
                clearSelection();
                moveHistory.clear();
                isPlayerTurn = player1.getColor() == PlayerColor::White;
                stockfish.newGame();
            }

            overlayType = OverlayType::None;
        }

        return;
    }

    if (draw.getInput().isResignClicked()) {
        resign(player2);
        return;
    }

    if (draw.getInput().isOfferDrawClicked()) {
        offerDraw(player2);
        return;
    }

    if (draw.getInput().isShowResultsClicked()) {
        showResults();
        return;
    }

    int x = -1;
    int y = -1;

    if (!draw.getInput().getClickedBoardCell(x, y, playerPlaysWhite)) return;

    Piece* clickedPiece = board.getPiece(x, y);

    if (!pieceSelected) {
        PieceColor playerPieceColor = playerPlaysWhite ? PieceColor::White : PieceColor::Black;

        if (clickedPiece != nullptr && clickedPiece->getColor() == playerPieceColor) {
            pieceSelected = true;
            selectedX = x;
            selectedY = y;
        }
        return;
    }

    if (x == selectedX && y == selectedY) {
        clearSelection();
        return;
    }

    Move newMove;
    newMove.setFromX(selectedX);
    newMove.setFromY(selectedY);
    newMove.setToX(x);
    newMove.setToY(y);

    if (validateMove(newMove)) {
        board.update(newMove);
        moveHistory.push_back(moveToUci(newMove));
        clearSelection();
        isPlayerTurn = false;
    }
}

void Game::makeStockfishMove() {
    if (overlayType != OverlayType::None) return;
    if (isPlayerTurn) return;

    std::string bestMove = stockfish.move(moveHistory);

    if (bestMove.empty() || bestMove == "(none)" || bestMove == "0000") {
        overlayType = OverlayType::Results;
        isPlayerTurn = true;
        return;
    }

    Move move = uciToMove(bestMove);
    board.update(move);
    moveHistory.push_back(bestMove);
    isPlayerTurn = true;
}

void Game::startGame(bool playerPlaysWhite, int stockfishElo) {
    this->playerPlaysWhite = playerPlaysWhite;
    stockfish.setElo((StockfishElo)stockfishElo);
    player1.setColor(playerPlaysWhite ? PlayerColor::White : PlayerColor::Black);
    player2.setColor(playerPlaysWhite ? PlayerColor::Black : PlayerColor::White);
    isPlayerTurn = playerPlaysWhite;
    overlayType = OverlayType::None;
    clearSelection();
    moveHistory.clear();
    board = Board();
    stockfish.newGame();

    while (!draw.shouldClose()) {
        update();
        draw.render(board, pieceSelected, selectedX, selectedY, overlayType, player2.getName(), this->playerPlaysWhite);

        if (!isPlayerTurn) {
            std::this_thread::sleep_for(std::chrono::milliseconds(STOCKFISH_MOVE_TIME_MS));
            makeStockfishMove();
        }
    }

    draw.closeWindow();
}

void Game::mainMenu() {
    draw.initWindow();

    bool playerPlaysWhite = PLAYER_PLAYS_WHITE;
    int selectedElo = STOCKFISH_ELO;
    while (!draw.shouldClose()) {
        if (draw.getInput().isLeftMousePressed()) {
            if (draw.getInput().isStartGameClicked()) {
                startGame(playerPlaysWhite, selectedElo);
            } else if (draw.getInput().isSelectWhiteClicked()) {
                playerPlaysWhite = true;
                draw.mainMenu(true, selectedElo);
            } else if (draw.getInput().isSelectBlackClicked()) {
                playerPlaysWhite = false;
                draw.mainMenu(false, selectedElo);
            } else if (draw.getInput().isSelectEloClicked()) {
                selectedElo = getNextStockfishElo(selectedElo);
                draw.mainMenu(playerPlaysWhite, selectedElo);
            }
        } else {
            draw.mainMenu(playerPlaysWhite, selectedElo);
        }
    }
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

std::string Game::moveToUci(Move move) {
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
    overlayType = OverlayType::Results;
}

void Game::resign(Player player) {
    overlayType = OverlayType::Resign;
}

void Game::offerDraw(Player player) {
    // TODO
    overlayType = OverlayType::Draw;
}

bool Game::validateMove(Move move) {
    // Šita funkcija jau turi prieigą prie board, moveHistory, player1, player2, ko turėtų užtekti
    // board turi piece, turi hasMoved, o en passant galima remtis moveHistory
    // kol kas nėra promote ir endGame
    // TODO
    return true;
}
