#pragma once
#include <bits/stdc++.h>

#include "Move.hpp"
#include "Piece.hpp"

class Piece;

class Board {

    private:
        Piece* squares[8][8];

    public:
        Piece* getPiece(int x, int y);
        void update(Move move);
};
