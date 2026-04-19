#include "../Config.hpp"
#include "UserInput.hpp"

Input::Input(const Rectangle& resignButton,
             const Rectangle& offerDrawButton,
             const Rectangle& showResultsButton,
             const Rectangle& overlayButton,
             const Rectangle& selectWhiteButton,
             const Rectangle& selectBlackButton,
             const Rectangle& startGameButton)
    : resignButton(resignButton),
      offerDrawButton(offerDrawButton),
      showResultsButton(showResultsButton),
      overlayButton(overlayButton),
      selectWhiteButton(selectWhiteButton),
      selectBlackButton(selectBlackButton),
      startGameButton(startGameButton) {
}

bool Input::isLeftMousePressed() {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool Input::isResignClicked() {
    return CheckCollisionPointRec(GetMousePosition(), resignButton);
}

bool Input::isOfferDrawClicked() {
    return CheckCollisionPointRec(GetMousePosition(), offerDrawButton);
}

bool Input::isShowResultsClicked() {
    return CheckCollisionPointRec(GetMousePosition(), showResultsButton);
}

bool Input::isOverlayButtonClicked() {
    return CheckCollisionPointRec(GetMousePosition(), overlayButton);
}

bool Input::isSelectWhiteClicked() {
    return CheckCollisionPointRec(GetMousePosition(), selectWhiteButton);
}

bool Input::isSelectBlackClicked() {
    return CheckCollisionPointRec(GetMousePosition(), selectBlackButton);
}

bool Input::isStartGameClicked() {
    return CheckCollisionPointRec(GetMousePosition(), startGameButton);
}

bool Input::getClickedBoardCell(int& x, int& y, bool playerPlaysWhite) {
    Vector2 mousePos = GetMousePosition();

    int screenX = (int)(mousePos.x / (WIDTH / 8.0f));
    int screenY = (int)(mousePos.y / (LENGTH / 8.0f));

    if (playerPlaysWhite) {
        x = screenX;
        y = screenY;
    } else {
        x = 7 - screenX;
        y = 7 - screenY;
    }

    return true;
}
