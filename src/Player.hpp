#pragma once

#include <string>

#include "Move.hpp"

enum class PlayerColor {
    White,
    Black
};

class Player {

    private:
        std::string name;
        PlayerColor color;

    public:
        void makeMove(Move move);
        void resign();
        void offerDraw();
};
