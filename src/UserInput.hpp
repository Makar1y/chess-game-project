#pragma once

#include "raylib.h"

class Input {
public:
    Input(const Rectangle& resignButton,
          const Rectangle& offerDrawButton,
          const Rectangle& showResultsButton,
          const Rectangle& overlayYesButton,
          const Rectangle& overlayNoButton,
          const Rectangle& selectWhiteButton,
          const Rectangle& selectBlackButton,
          const Rectangle& startGameButton,
          const Rectangle& selectEloButton,
          const Rectangle& undoButton,
          const Rectangle& backToGameButton,
          const Rectangle& exitToMenuButton,
          const Rectangle& newGameButton,
          const Rectangle& exitButton);


    bool isLeftMousePressed();
    bool isAnyKeyPressed();
    bool isResignClicked();
    bool isOfferDrawClicked();
	bool isUndoClicked();
    bool isShowResultsClicked();
    bool isOverlayYesClicked();
    bool isOverlayNoClicked();
    bool isSelectWhiteClicked();
    bool isSelectBlackClicked();
    bool isStartGameClicked();
    bool isSelectEloClicked();
    bool isBackToGameClicked();
    bool isExitToMenuClicked();
    bool isNewGameClicked();
    bool getClickedBoardCell(int& x, int& y, bool playerPlaysWhite);
    bool isExitClicked();

private:
    const Rectangle& resignButton;
    const Rectangle& offerDrawButton;
    const Rectangle& showResultsButton;
    const Rectangle& overlayYesButton;
    const Rectangle& overlayNoButton;
    const Rectangle& selectWhiteButton;
    const Rectangle& selectBlackButton;
    const Rectangle& startGameButton;
    const Rectangle& selectEloButton;
    const Rectangle& undoButton;
    const Rectangle& backToGameButton;
    const Rectangle& exitToMenuButton;
    const Rectangle& newGameButton;
    const Rectangle& exitButton;
};
