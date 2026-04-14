#pragma once

#include "Move.hpp"
#include "Piece.hpp"

#include <memory>

class Piece;

class Board {
private:
    std::unique_ptr<Piece> squares[8][8];

public:
    Board();
    Piece* getPiece(int x, int y);
    void update(Move move);
};
