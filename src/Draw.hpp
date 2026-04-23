#pragma once

#include <string>
#include <set>
#include <vector>

#include "Board.hpp"
#include "UserInput.hpp"
#include "raylib.h"

enum class OverlayType {
    None,
    Resign,
    Draw,
    Results
};

class Draw {
public:
    Draw();

    void initWindow();
    void mainMenu(bool playerPlaysWhite, int stockfishElo);
    bool shouldClose();
    void closeWindow();

    Input& getInput();

    void render(Board& board, bool pieceSelected, int selectedX, int selectedY, 
        std::set<std::pair<int, int>>& possibleMoves, std::set<std::pair<int, int>>& possibleCaptures,
        Move* lastMove, bool hasLastMove, OverlayType overlayType, std::string playerName, bool playerPlaysWhite,
        const std::vector<std::string>& moveHistory, const std::string& winnerName, const std::string& winReason);

private:
    void renderOverlay(OverlayType overlayType, float buttonRoundness, int buttonSegments,
        const std::vector<std::string>& moveHistory, const std::string& winnerName, const std::string& winReason);
    static void confirmationOverlay(const Rectangle& overlayRect, const Rectangle& overlayYesButton, const Rectangle& overlayNoButton, const Font& uiFont22, const char* messageText, bool overlayYesPressed, bool overlayNoPressed, float buttonRoundness, int buttonSegments);
    void showMoveHistory(const std::vector<std::string>& moveHistory);
    void showResults(const std::string& winnerName, const std::string& winReason, int movesMade);
    void loadResources();
    void unloadResources();

    bool resourcesLoaded;

    Font uiFont22;
    Font uiFont36;

    Texture2D boardTexture;
    Texture2D stockfishTexture;

    Texture2D b_bishop;
    Texture2D b_king;
    Texture2D b_knight;
    Texture2D b_pawn;
    Texture2D b_queen;
    Texture2D b_rook;

    Texture2D w_bishop;
    Texture2D w_king;
    Texture2D w_knight;
    Texture2D w_pawn;
    Texture2D w_queen;
    Texture2D w_rook;

    Rectangle resignButton;
    Rectangle offerDrawButton;
    Rectangle showResultsButton;
	Rectangle undoButton;
    Rectangle overlayRect;
    Rectangle overlayYesButton;
    Rectangle overlayNoButton;
    Rectangle startGameButton;
    Rectangle selectWhiteButton;
    Rectangle selectBlackButton;
    Rectangle selectEloButton;

    Input input;
};
