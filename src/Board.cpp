#include "Board.hpp"
#include "Piece.hpp"

Board::Board() {

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            squares[y][x] = nullptr;
        }
    }

    squares[0][0] = std::make_unique<Piece>(PieceColor::Black, PieceType::Rook);
    squares[0][1] = std::make_unique<Piece>(PieceColor::Black, PieceType::Knight);
    squares[0][2] = std::make_unique<Piece>(PieceColor::Black, PieceType::Bishop);
    squares[0][3] = std::make_unique<Piece>(PieceColor::Black, PieceType::Queen);
    squares[0][4] = std::make_unique<Piece>(PieceColor::Black, PieceType::King);
    squares[0][5] = std::make_unique<Piece>(PieceColor::Black, PieceType::Bishop);
    squares[0][6] = std::make_unique<Piece>(PieceColor::Black, PieceType::Knight);
    squares[0][7] = std::make_unique<Piece>(PieceColor::Black, PieceType::Rook);

    for (int i = 0; i < 8; i++) {
        squares[1][i] = std::make_unique<Piece>(PieceColor::Black, PieceType::Pawn);
    }

    for (int i = 0; i < 8; i++) {
        squares[6][i] = std::make_unique<Piece>(PieceColor::White, PieceType::Pawn);
    }

    squares[7][0] = std::make_unique<Piece>(PieceColor::White, PieceType::Rook);
    squares[7][1] = std::make_unique<Piece>(PieceColor::White, PieceType::Knight);
    squares[7][2] = std::make_unique<Piece>(PieceColor::White, PieceType::Bishop);
    squares[7][3] = std::make_unique<Piece>(PieceColor::White, PieceType::Queen);
    squares[7][4] = std::make_unique<Piece>(PieceColor::White, PieceType::King);
    squares[7][5] = std::make_unique<Piece>(PieceColor::White, PieceType::Bishop);
    squares[7][6] = std::make_unique<Piece>(PieceColor::White, PieceType::Knight);
    squares[7][7] = std::make_unique<Piece>(PieceColor::White, PieceType::Rook);

}

Piece* Board::getPiece(int x, int y) {
    return squares[y][x].get();
}

void Board::update(Move move) {
    int fromX = move.getFromX();
    int fromY = move.getFromY();
    int toX = move.getToX();
    int toY = move.getToY();

    if (!squares[fromY][fromX]) return;

    squares[toY][toX] = std::move(squares[fromY][fromX]);
}
