#pragma once

#include <string>
#include <cmath>

#include "Board.hpp"
#include "UserInput.hpp"
#include "raylib.h"

enum class OverlayType : int;

class Draw {
public:
    Draw();

    void initWindow();
    void mainMenu(bool playerPlaysWhite, int stockfishElo);
    bool shouldClose();
    void closeWindow();

    Input& getInput();

    void render(Board& board, bool pieceSelected, int selectedX, int selectedY, OverlayType overlayType, std::string playerName, bool playerPlaysWhite);

private:
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
    Rectangle overlayRect;
    Rectangle overlayButton;
    Rectangle startGameButton;
    Rectangle selectWhiteButton;
    Rectangle selectBlackButton;
    Rectangle selectEloButton;

    Input input;
};
