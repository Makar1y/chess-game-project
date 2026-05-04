#pragma once

#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <optional>
#include  <stack>

#include "../Config.hpp"
#include "Board.hpp"
#include "Draw.hpp"
#include "Move.hpp"
#include "Player.hpp"
#include "Stockfish.hpp"
#include "Audio.hpp"


class Game {
private:
    enum class ScreenState {
        MainMenu,
        InGame
    };

    Draw draw;
    Board board;
    Player player1;
    Player player2;
    Stockfish stockfish;
    Audio audio;
    int selectedElo = STOCKFISH_ELO;
    float selectedTimeControlSeconds = PLAYER_TIME;
    int halfMoves = 0;

    OverlayType overlayType = OverlayType::None;
    std::string overlayMessage;

    std::string winnerName;
    std::string winReason;

    bool pieceSelected = false;
    int selectedX = -1;
    int selectedY = -1;

    std::set<std::pair<int, int>> possibleMoves;
    std::set<std::pair<int, int>> possibleCaptures;

    float playerTimeLeftSeconds = PLAYER_TIME;

    bool playerPlaysWhite = PLAYER_PLAYS_WHITE;
    bool isPlayerTurn = true;
    std::vector<std::string> moveHistory;
    Move lastMove;
    bool hasLastMove = false;
    ScreenState screenState = ScreenState::MainMenu;

    void resetGameState();

    void makeStockfishMove();
    std::string moveToUci(Move& move);
    Move uciToMove(const std::string& uciMove);

    void getPossibleMoves(int x, int y);
    void clearPossibleMoves();
    bool isCapture(int fx, int fy, int tx, int ty);
    Move convertToMove(int fx, int fy, int tx, int ty);
    void unpackMove(Move& move, int& fx, int& fy, int& tx, int& ty);
    bool checkForCheck(PieceColor color);
    bool isThereNoCheck(Move& move);
    bool hasAnyLegalMove(PieceColor color);
    void checkForGameEnd(PieceColor color);

    bool isValidPawnMove(int fx, int fy, int tx, int ty);
    bool isValidRockMove(int fx, int fy, int tx, int ty);
    bool isValidBishopMove(int fx, int fy, int tx, int ty);
    bool isValidQueenMove(int fx, int fy, int tx, int ty);
    bool isValidKnightMove(int fx, int fy, int tx, int ty);
    bool isValidKingMove(int fx, int fy, int tx, int ty);

    bool areSameColorPieces(int fx, int fy, int tx, int ty);
    bool isPathClear(int fx, int fy, int tx, int ty);
    bool wasItcastle(Move& move);
    void moveRook(Move& move);
    bool wasIsEnPassant(Move& move);
    void removeEnPassantPawn(Move& move);
    bool wasItPawnPromotio(Move& move);
    void promotePawn(Move& move);
    bool wasPawnMove(int x, int y);


public:
    Game();

    void update();
    void startGame(bool playerPlaysWhite, int stockfishElo, float timeControlSeconds);
    void mainMenu();
    void clearSelection();
    void showResults();
    void showMoveHistory();
    void goToMainMenu();
    void startNewGame();
    void resign();
    void offerDraw();
    void offerdaw();
    bool validateMove(Move& move);
    int getNextStockfishElo(int currentElo);
    int getPreviousStockfishElo(int currentElo);
    float getNextTimeControl(float currentTimeControlSeconds);
    float getPreviousTimeControl(float currentTimeControlSeconds);
    void undoLastMove();
    std::stack<Move> undoStack;
};
