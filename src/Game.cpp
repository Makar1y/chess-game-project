#include "Game.hpp"
#include "Draw.hpp"

static Draw draw;

Game::Game()
    : player1("Player 1", PlayerColor::White),
      player2("Stockfish", PlayerColor::Black) {
}

void Game::update() {
    if (!draw.isLeftMousePressed()) return;

    if (overlayType != OverlayType::None) {
        if (draw.isOverlayButtonClicked()) {
            if (overlayType == OverlayType::Resign || overlayType == OverlayType::Draw) {
                board = Board();
                clearSelection();
            }

            overlayType = OverlayType::None;
        }

        return;
    }

    if (draw.isResignClicked()) {
        resign(player2);
        return;
    }

    if (draw.isOfferDrawClicked()) {
        offerDraw(player2);
        return;
    }

    if (draw.isShowResultsClicked()) {
        showResults();
        return;
    }

    int x = -1;
    int y = -1;

    if (!draw.getClickedBoardCell(x, y)) return;

    Piece* clickedPiece = board.getPiece(x, y);

    if (!pieceSelected) {
        if (clickedPiece != nullptr) {
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
        clearSelection();
    }
}

void Game::startGame() {
    draw.initWindow();

    while (!draw.shouldClose()) {
        update();
        draw.render(board, pieceSelected, selectedX, selectedY, overlayType, player2.getName());
    }

    draw.closeWindow();
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
    //TODO
    overlayType = OverlayType::Resign;
}

void Game::offerDraw(Player player) {
    //TODO
    overlayType = OverlayType::Draw;
}

bool Game::validateMove(Move move) {
    //TODO
    return true;
}
