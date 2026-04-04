#pragma once
#include <bits/stdc++.h>
#include <raylib.h>

enum class PieceType {
    None,
    Pawn,
    Knight,
    Bishop,
    Ropk,
    Queen,
    King
};

class Piece {

    private:
        Color color;
        PieceType type;

    public:

        Color getColor();
        PieceType getType();
};
