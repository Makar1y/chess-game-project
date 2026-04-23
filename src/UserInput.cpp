#include "../Config.hpp"
#include "UserInput.hpp"

Input::Input(const Rectangle& resignButton,
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
             const Rectangle& newGameButton)
    : resignButton(resignButton),
      offerDrawButton(offerDrawButton),
      showResultsButton(showResultsButton),
      overlayYesButton(overlayYesButton),
      overlayNoButton(overlayNoButton),
      selectWhiteButton(selectWhiteButton),
      selectBlackButton(selectBlackButton),
      startGameButton(startGameButton),
      selectEloButton(selectEloButton),
      undoButton(undoButton),
      backToGameButton(backToGameButton),
      exitToMenuButton(exitToMenuButton),
      newGameButton(newGameButton) {
}

bool Input::isLeftMousePressed() {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool Input::isAnyKeyPressed() {
    return GetKeyPressed() != 0;
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

bool Input::isOverlayYesClicked() {
    return CheckCollisionPointRec(GetMousePosition(), overlayYesButton);
}

bool Input::isOverlayNoClicked() {
    return CheckCollisionPointRec(GetMousePosition(), overlayNoButton);
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

bool Input::isSelectEloClicked() {
    return CheckCollisionPointRec(GetMousePosition(), selectEloButton);
}

bool Input::isUndoClicked() {
    return CheckCollisionPointRec(GetMousePosition(), undoButton);
}

bool Input::isBackToGameClicked() {
    return CheckCollisionPointRec(GetMousePosition(), backToGameButton);
}

bool Input::isExitToMenuClicked() {
    return CheckCollisionPointRec(GetMousePosition(), exitToMenuButton);
}

bool Input::isNewGameClicked() {
    return CheckCollisionPointRec(GetMousePosition(), newGameButton);
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
