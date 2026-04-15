#include "Piece.hpp"

Piece::Piece(PieceColor color, PieceType type) : color(color), type(type), hasMoved(false) {
}

PieceColor Piece::getColor() {
    return color;
}

PieceType Piece::getType() {
    return type;
}

bool Piece::getHasMoved() {
    return hasMoved;
}

void Piece::setHasMoved(bool value) {
    hasMoved = value;
}
