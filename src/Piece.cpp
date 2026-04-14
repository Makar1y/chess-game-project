#include "Piece.hpp"

Piece::Piece(PieceColor color, PieceType type) : color(color), type(type) {
}

PieceColor Piece::getColor() {
    return color;
}

PieceType Piece::getType() {
    return type;
}
