#pragma once

#include <raylib.h>

#include "Board.hpp"
#include "Move.hpp"
#include "Player.hpp"

enum class OverlayType {
    None,
    Resign,
    Draw,
    Results
};

class Game {
    private:
        Board board;
        Player player1;
        Player player2;
        OverlayType overlayType = OverlayType::None;

        bool pieceSelected = false;
        int selectedX = -1;
        int selectedY = -1;

    public:
        Game();

        void clearSelection();
        bool validateMove(Move move);
        void update();
        void startGame();
        void resign(Player Player);
        void offerDraw(Player player);
        void showResults();
};
