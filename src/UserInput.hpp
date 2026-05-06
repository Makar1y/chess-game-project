#pragma once

#include "raylib.h"

#include <string>
#include <cctype>

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
          const Rectangle& selectEloLeftButton,
          const Rectangle& selectEloRightButton,
          const Rectangle& selectTimeButton,
          const Rectangle& selectTimeLeftButton,
          const Rectangle& selectTimeRightButton,
          const Rectangle& undoButton,
          const Rectangle& backToGameButton,
          const Rectangle& exitToMenuButton,
          const Rectangle& newGameButton,
          const Rectangle& exitButton,
          const Rectangle& pvpMenuButton,
          const Rectangle& hostGameButton,
          const Rectangle& joinGameButton,
          const Rectangle& ipInputBox,
          const Rectangle& portInputBox,
          const Rectangle& backButton);

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
    bool isSelectEloLeftClicked();
    bool isSelectEloRightClicked();
    bool isSelectTimeClicked();
    bool isSelectTimeLeftClicked();
    bool isSelectTimeRightClicked();
    bool isBackToGameClicked();
    bool isExitToMenuClicked();
    bool isNewGameClicked();
    bool getClickedBoardCell(int& x, int& y, bool playerPlaysWhite);
    bool isExitClicked();
    bool isPvpMenuClicked();
    bool isHostGameClicked();
    bool isJoinGameClicked();
    bool isIpInputClicked();
    bool isPortInputClicked();
    bool isBackClicked();

    void handleTextInput(std::string& text, int maxLength, bool onlyDigits);

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
    const Rectangle& selectEloLeftButton;
    const Rectangle& selectEloRightButton;
    const Rectangle& selectTimeButton;
    const Rectangle& selectTimeLeftButton;
    const Rectangle& selectTimeRightButton;
    const Rectangle& undoButton;
    const Rectangle& backToGameButton;
    const Rectangle& exitToMenuButton;
    const Rectangle& newGameButton;
    const Rectangle& exitButton;
    const Rectangle& pvpMenuButton;
    const Rectangle& hostGameButton;
    const Rectangle& joinGameButton;
    const Rectangle& ipInputBox;
    const Rectangle& portInputBox;
    const Rectangle& backButton;
};
