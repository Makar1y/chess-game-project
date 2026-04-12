#pragma once

#include <string>

#include "Board.hpp"
#include "Game.hpp"
#include "raylib.h"

class Draw {
public:
    Draw();

    void initWindow();
    bool shouldClose();
    void closeWindow();

    bool isLeftMousePressed();
    bool isResignClicked();
    bool isOfferDrawClicked();
    bool isShowResultsClicked();
    bool isOverlayButtonClicked();
    bool getClickedBoardCell(int& x, int& y);

    void render(Board& board, bool pieceSelected, int selectedX, int selectedY, OverlayType overlayType, std::string playerName);

private:
    void loadResources();
    void unloadResources();

    bool resourcesLoaded;

    Font uiFont;

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
};
