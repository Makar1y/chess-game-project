#pragma once

#include "raylib.h"

class Input {
public:
    Input(const Rectangle& resignButton,
          const Rectangle& offerDrawButton,
          const Rectangle& showResultsButton,
          const Rectangle& overlayButton);

    bool isLeftMousePressed();
    bool isResignClicked();
    bool isOfferDrawClicked();
    bool isShowResultsClicked();
    bool isOverlayButtonClicked();
    bool getClickedBoardCell(int& x, int& y, bool playerPlaysWhite);

private:
    const Rectangle& resignButton;
    const Rectangle& offerDrawButton;
    const Rectangle& showResultsButton;
    const Rectangle& overlayButton;
};
