#include "Game.hpp"
#include "Draw.hpp"

Game::Game()
    : player1("Player 1", PLAYER_PLAYS_WHITE ? PlayerColor::White : PlayerColor::Black),
      player2("Stockfish", PLAYER_PLAYS_WHITE ? PlayerColor::Black : PlayerColor::White),
      stockfish((StockfishElo)STOCKFISH_ELO) {
    stockfish.newGame();
    isPlayerTurn = PLAYER_PLAYS_WHITE;
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
                isPlayerTurn = PLAYER_PLAYS_WHITE;
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

    if (!draw.getInput().getClickedBoardCell(x, y, PLAYER_PLAYS_WHITE)) return;

    Piece* clickedPiece = board.getPiece(x, y);

    if (!pieceSelected) {
        PieceColor playerPieceColor = PLAYER_PLAYS_WHITE ? PieceColor::White : PieceColor::Black;

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

void Game::startGame() {
    draw.initWindow();

    while (!draw.shouldClose()) {
        update();
        draw.render(board, pieceSelected, selectedX, selectedY, overlayType, player2.getName(), PLAYER_PLAYS_WHITE);

        if (!isPlayerTurn) {
            std::this_thread::sleep_for(std::chrono::milliseconds(STOCKFISH_MOVE_TIME_MS));
            makeStockfishMove();
        }
    }

    draw.closeWindow();
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
    // TODO
    overlayType = OverlayType::Resign;
}

void Game::offerDraw(Player player) {
    // TODO
    overlayType = OverlayType::Draw;
}

bool Game::validateMove(Move move) {
    // TODO
    return true;
}
