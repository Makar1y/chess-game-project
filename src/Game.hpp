#pragma once

#include <raylib.h>

#include "Board.hpp"
#include "Move.hpp"
#include "Player.hpp"

enum class GameState {
    NotStarted,
    Started,
    Ended
};

class Game {

    private:
        Board board;
        Player player1;
        Player player2;
        GameState state;

        bool validateMove(Move move);
        void updateBoard(Move move);

    public:
        void startGame();
        bool makeMove(Move move);
        void resign(Player Player);
        void offerDraw(Player player);
        void showResults();
};
