#pragma once
#include <bits/stdc++.h>
#include <raylib.h>

#include "Move.hpp"

class Player {

    private:
        std::string name;
        Color color;

    public:
        void makeMove(Move move);
        void resign();
        void offerDraw();
};
