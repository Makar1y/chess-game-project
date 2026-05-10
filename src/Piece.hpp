#pragma once

enum class PieceType {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
    None
};

enum class PieceColor {
    White,
    Black
};

class Piece {

    private:
        PieceColor color;
        PieceType type;
        bool hasMoved = false;

    public:
        Piece(PieceColor color, PieceType type);


        PieceColor getColor();

        PieceType getType();

        bool getHasMoved();
        void setHasMoved(bool value);
        void setType(PieceType type);
};
