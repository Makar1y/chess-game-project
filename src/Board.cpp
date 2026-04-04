#include "Board.hpp"
#include "Piece.hpp"

Piece *Board::getPiece(int x, int y) {
    return squares[x][y];
}

void update(Move move) {
    //TODO
}
