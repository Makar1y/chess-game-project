#pragma once

#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "../Config.hpp"
#include "Board.hpp"
#include "Draw.hpp"
#include "Move.hpp"
#include "Player.hpp"
#include "Stockfish.hpp"

enum class OverlayType {
    None,
    Resign,
    Draw,
    Results
};

class Game {
private:
    Draw draw;
    Board board;
    Player player1;
    Player player2;
    Stockfish stockfish;

    OverlayType overlayType = OverlayType::None;

    bool pieceSelected = false;
    int selectedX = -1;
    int selectedY = -1;

    bool playerPlaysWhite = PLAYER_PLAYS_WHITE;
    bool isPlayerTurn = true;
    std::vector<std::string> moveHistory;

    void makeStockfishMove();
    std::string moveToUci(Move move);
    Move uciToMove(const std::string& uciMove);

public:
    Game();

    void update();
    void startGame(bool playerPlaysWhite, int stockfishElo);
    void mainMenu();
    void clearSelection();
    void showResults();
    void resign(Player player);
    void offerDraw(Player player);
    bool validateMove(Move move);
    int getNextStockfishElo(int currentElo);
};
