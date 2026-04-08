#pragma once

enum class PieceType {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

enum class PieceColor {
    White,
    Black
};

class Piece {

    private:
        PieceColor color;
        PieceType type;

    public:
        Piece(PieceColor color, PieceType type);

        PieceColor getColor();
        PieceType getType();
};
