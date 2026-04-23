#include "../Config.hpp"
#include "Draw.hpp"
#include "Game.hpp"

#include <cmath>
#include <cstdio>

static std::string formatClock(float timeLeftSeconds) {
    int totalSeconds = (int)std::ceil(timeLeftSeconds);
    if(totalSeconds < 0) totalSeconds = 0;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
    return std::string(buffer);
}

Draw::Draw()
    : input(resignButton, offerDrawButton, showResultsButton, overlayYesButton, overlayNoButton, selectWhiteButton, selectBlackButton, startGameButton, selectEloButton, undoButton, backToGameButton, exitToMenuButton, newGameButton, exitButton) {
    resourcesLoaded = false;

    const float overlayWidth              = 380.0f;
    const float overlayHeight             = 210.0f;
    const float overlayButtonWidth        = 120.0f;
    const float overlayButtonHeight       = 46.0f;
    const float overlayButtonSpacing      = 20.0f;
    const float overlayButtonTopOffset    = 138.0f;

    resignButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    undoButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    offerDrawButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    showResultsButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    backToGameButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    exitToMenuButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    newGameButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    exitButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };

    // main menu buttons
    startGameButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectWhiteButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectBlackButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectEloButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };

    overlayRect = Rectangle{
        ((float)(WIDTH + PANEL_WIDTH) - overlayWidth) * 0.5f,
        ((float)LENGTH - overlayHeight) * 0.5f,
        overlayWidth,
        overlayHeight
    };

    overlayYesButton = Rectangle{
        overlayRect.x + (overlayRect.width - (overlayButtonWidth * 2.0f + overlayButtonSpacing)) * 0.5f,
        overlayRect.y + overlayButtonTopOffset,
        overlayButtonWidth,
        overlayButtonHeight
    };

    overlayNoButton = Rectangle{
        overlayYesButton.x + overlayButtonWidth + overlayButtonSpacing,
        overlayYesButton.y,
        overlayButtonWidth,
        overlayButtonHeight
    };
}

void Draw::loadResources() {
    if (resourcesLoaded) return;

    uiFont22 = LoadFontEx("Fonts/Rubik-SemiBold.ttf", 22, 0, 0);
    uiFont36 = LoadFontEx("Fonts/Rubik-SemiBold.ttf", 36, 0, 0);

    SetTextureFilter(uiFont22.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(uiFont36.texture, TEXTURE_FILTER_BILINEAR);

    boardTexture = LoadTexture("Textures/board.png");
    stockfishTexture = LoadTexture("Textures/stockfish.png");

    b_bishop = LoadTexture("Textures/bb.png");
    b_king   = LoadTexture("Textures/bk.png");
    b_knight = LoadTexture("Textures/bn.png");
    b_pawn   = LoadTexture("Textures/bp.png");
    b_queen  = LoadTexture("Textures/bq.png");
    b_rook   = LoadTexture("Textures/br.png");

    w_bishop = LoadTexture("Textures/wb.png");
    w_king   = LoadTexture("Textures/wk.png");
    w_knight = LoadTexture("Textures/wn.png");
    w_pawn   = LoadTexture("Textures/wp.png");
    w_queen  = LoadTexture("Textures/wq.png");
    w_rook   = LoadTexture("Textures/wr.png");

    SetTextureFilter(stockfishTexture, TEXTURE_FILTER_TRILINEAR);

    SetTextureFilter(b_bishop, TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(b_king,   TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(b_knight, TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(b_pawn,   TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(b_queen,  TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(b_rook,   TEXTURE_FILTER_TRILINEAR);

    SetTextureFilter(w_bishop, TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(w_king,   TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(w_knight, TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(w_pawn,   TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(w_queen,  TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(w_rook,   TEXTURE_FILTER_TRILINEAR);

    resourcesLoaded = true;
}

void Draw::unloadResources() {
    if (!resourcesLoaded) return;

    UnloadFont(uiFont22);
    UnloadFont(uiFont36);

    UnloadTexture(boardTexture);
    UnloadTexture(stockfishTexture);

    UnloadTexture(b_bishop);
    UnloadTexture(b_king);
    UnloadTexture(b_knight);
    UnloadTexture(b_pawn);
    UnloadTexture(b_queen);
    UnloadTexture(b_rook);

    UnloadTexture(w_bishop);
    UnloadTexture(w_king);
    UnloadTexture(w_knight);
    UnloadTexture(w_pawn);
    UnloadTexture(w_queen);
    UnloadTexture(w_rook);

    resourcesLoaded = false;
}

void Draw::initWindow() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH + PANEL_WIDTH, LENGTH, "Chess");
    SetTargetFPS(FPS);
    loadResources();
}

bool Draw::shouldClose() {
    return WindowShouldClose();
}

void Draw::closeWindow() {
    unloadResources();
    CloseWindow();
}

Input& Draw::getInput() {
    return input;
}

void Draw::showMoveHistory(const std::vector<std::string>& moveHistory) {
    // Use a larger overlay for move history with buttons
    const float historyOverlayWidth = 500.0f;
    const float historyOverlayHeight = 450.0f;
    Rectangle historyOverlayRect = {
        ((float)(WIDTH + PANEL_WIDTH) - historyOverlayWidth) * 0.5f,
        ((float)LENGTH - historyOverlayHeight) * 0.5f,
        historyOverlayWidth,
        historyOverlayHeight
    };

    DrawRectangle(0, 0, WIDTH + PANEL_WIDTH, LENGTH, Fade(BLACK, 0.25f));
    DrawRectangleRec(historyOverlayRect, RAYWHITE);
    DrawRectangleLinesEx(historyOverlayRect, 2, GRAY);

    const char* title = "Move History";
    Vector2 titleSize = MeasureTextEx(uiFont22, title, 22, 1);
    DrawTextEx(uiFont22, title, { historyOverlayRect.x + (historyOverlayRect.width - titleSize.x) * 0.5f, historyOverlayRect.y + 10 }, 22, 1, DARKGRAY);

    // Show result stats
    std::string movesStr = "Total Moves: " + std::to_string(moveHistory.size());
    DrawTextEx(uiFont22, movesStr.c_str(), { historyOverlayRect.x + 20, historyOverlayRect.y + 40 }, 18, 1, DARKGRAY);

    // Build move history text (show all moves)
    std::string historyText = "";
    for (size_t i = 0; i < moveHistory.size(); ++i) {
        if (i % 2 == 0) {
            historyText += std::to_string(i / 2 + 1) + ". ";
        }
        historyText += moveHistory[i] + " ";
        if (i % 2 != 0) historyText += "\n";
    }

    // Draw move history in a scrollable area (simplified - just show last 20 moves)
    std::string recentHistoryText = "";
    int movesToShow = std::min(20, (int)moveHistory.size());
    int startIdx = std::max(0, (int)moveHistory.size() - movesToShow);
    for (int i = startIdx; i < (int)moveHistory.size(); ++i) {
        if (i % 2 == 0) {
            recentHistoryText += std::to_string(i / 2 + 1) + ". ";
        }
        recentHistoryText += moveHistory[i] + " ";
        if (i % 2 != 0) recentHistoryText += "\n";
    }

    DrawTextEx(uiFont22, recentHistoryText.c_str(), { historyOverlayRect.x + 20, historyOverlayRect.y + 70 }, 18, 1, BLACK);

    // Draw buttons
    float buttonWidth = 160.0f;
    float buttonHeight = 40.0f;
    float buttonSpacing = 15.0f;
    float buttonsY = historyOverlayRect.y + historyOverlayRect.height - buttonHeight - 20;

    backToGameButton = {
        historyOverlayRect.x + (historyOverlayRect.width - (buttonWidth * 2 + buttonSpacing)) * 0.5f,
        buttonsY,
        buttonWidth,
        buttonHeight
    };

    // Back to game button
    Color backColor = input.isBackToGameClicked() ? Color{ 218, 218, 190, 255 } : Color{ 238, 238, 210, 255 };
    DrawRectangleRounded(backToGameButton, 0.2f, 8, backColor);
    const char* backText = "Back to Game";
    Vector2 backSize = MeasureTextEx(uiFont22, backText, 18, 1);
    DrawTextEx(uiFont22, backText, { backToGameButton.x + (backToGameButton.width - backSize.x) * 0.5f, backToGameButton.y + (backToGameButton.height - backSize.y) * 0.5f }, 18, 1, BLACK);

}

void Draw::showResults(const std::string& winnerName, const std::string& winReason, int movesMade, const std::vector<std::string>& moveHistory) {
    // Use a larger overlay for results with move history and buttons
    const float resultsOverlayWidth = 500.0f;
    const float resultsOverlayHeight = 500.0f;
    Rectangle resultsOverlayRect = {
        ((float)(WIDTH + PANEL_WIDTH) - resultsOverlayWidth) * 0.5f,
        ((float)LENGTH - resultsOverlayHeight) * 0.5f,
        resultsOverlayWidth,
        resultsOverlayHeight
    };

    DrawRectangle(0, 0, WIDTH + PANEL_WIDTH, LENGTH, Fade(BLACK, 0.25f));
    DrawRectangleRec(resultsOverlayRect, RAYWHITE);
    DrawRectangleLinesEx(resultsOverlayRect, 2, GRAY);

    const char* title = "Game Results";
    Vector2 titleSize = MeasureTextEx(uiFont22, title, 22, 1);
    DrawTextEx(uiFont22, title, { resultsOverlayRect.x + (resultsOverlayRect.width - titleSize.x) * 0.5f, resultsOverlayRect.y + 10 }, 22, 1, DARKGRAY);

    std::string winnerStr = "Winner: " + winnerName;
    std::string reasonStr = "Reason: " + winReason;
    std::string movesStr = "Total moves: " + std::to_string(movesMade);

    DrawTextEx(uiFont22, winnerStr.c_str(), { resultsOverlayRect.x + 20, resultsOverlayRect.y + 50 }, 20, 1, BLACK);
    DrawTextEx(uiFont22, reasonStr.c_str(), { resultsOverlayRect.x + 20, resultsOverlayRect.y + 80 }, 20, 1, BLACK);
    DrawTextEx(uiFont22, movesStr.c_str(), { resultsOverlayRect.x + 20, resultsOverlayRect.y + 110 }, 20, 1, BLACK);

    // Show move history
    const char* historyTitle = "Move History:";
    DrawTextEx(uiFont22, historyTitle, { resultsOverlayRect.x + 20, resultsOverlayRect.y + 145 }, 18, 1, DARKGRAY);

    std::string recentHistoryText = "";
    int movesToShow = std::min(16, (int)moveHistory.size());
    int startIdx = std::max(0, (int)moveHistory.size() - movesToShow);
    for (int i = startIdx; i < (int)moveHistory.size(); ++i) {
        if (i % 2 == 0) {
            recentHistoryText += std::to_string(i / 2 + 1) + ". ";
        }
        recentHistoryText += moveHistory[i] + " ";
        if (i % 2 != 0) recentHistoryText += "\n";
    }

    DrawTextEx(uiFont22, recentHistoryText.c_str(), { resultsOverlayRect.x + 20, resultsOverlayRect.y + 175 }, 18, 1, BLACK);

    // Draw buttons
    float buttonWidth = 160.0f;
    float buttonHeight = 40.0f;
    float buttonSpacing = 15.0f;
    float buttonsY = resultsOverlayRect.y + resultsOverlayRect.height - buttonHeight - 20;

    exitToMenuButton = {
        resultsOverlayRect.x + (resultsOverlayRect.width - (buttonWidth * 2 + buttonSpacing)) * 0.5f,
        buttonsY,
        buttonWidth,
        buttonHeight
    };

    newGameButton = {
        exitToMenuButton.x + buttonWidth + buttonSpacing,
        buttonsY,
        buttonWidth,
        buttonHeight
    };

    // Draw Exit to Main Menu button
    Color exitColor = input.isExitToMenuClicked() ? Color{ 218, 218, 190, 255 } : Color{ 238, 238, 210, 255 };
    DrawRectangleRounded(exitToMenuButton, 0.2f, 8, exitColor);
    const char* exitText = "Exit to Menu";
    Vector2 exitSize = MeasureTextEx(uiFont22, exitText, 18, 1);
    DrawTextEx(uiFont22, exitText, { exitToMenuButton.x + (exitToMenuButton.width - exitSize.x) * 0.5f, exitToMenuButton.y + (exitToMenuButton.height - exitSize.y) * 0.5f }, 18, 1, BLACK);

    // Draw New Game button
    Color newGameColor = input.isNewGameClicked() ? Color{ 218, 218, 190, 255 } : Color{ 238, 238, 210, 255 };
    DrawRectangleRounded(newGameButton, 0.2f, 8, newGameColor);
    const char* newGameText = "New Game";
    Vector2 newGameSize = MeasureTextEx(uiFont22, newGameText, 18, 1);
    DrawTextEx(uiFont22, newGameText, { newGameButton.x + (newGameButton.width - newGameSize.x) * 0.5f, newGameButton.y + (newGameButton.height - newGameSize.y) * 0.5f }, 18, 1, BLACK);
}

void Draw::confirmationOverlay(const Rectangle& overlayRect, const Rectangle& overlayYesButton, const Rectangle& overlayNoButton,
    const char* messageText, bool overlayYesPressed, bool overlayNoPressed, float buttonRoundness, int buttonSegments) {
    DrawRectangle(0, 0, WIDTH + PANEL_WIDTH, LENGTH, Fade(BLACK, 0.25f));
    DrawRectangleRec(overlayRect, RAYWHITE);
    DrawRectangleLinesEx(overlayRect, 2, GRAY);

    constexpr float bodyFontSize = 22.0f;
    const float messageY = overlayRect.y + 58.0f;

    const Vector2 messageSize = MeasureTextEx(uiFont22, messageText, bodyFontSize, 1);
    const Vector2 yesSize = MeasureTextEx(uiFont22, "Yes", bodyFontSize, 1);
    const Vector2 noSize = MeasureTextEx(uiFont22, "No", bodyFontSize, 1);

    DrawTextEx(uiFont22, messageText,
        {
            std::round(overlayRect.x + (overlayRect.width - messageSize.x) * 0.5f),
            std::round(messageY)
        },
        bodyFontSize, 1, DARKGRAY);

    const Color yesColor = overlayYesPressed
        ? Color{ 218, 218, 190, 255 }
        : Color{ 238, 238, 210, 255 };
    const Color noColor = overlayNoPressed
        ? Color{ 218, 218, 190, 255 }
        : Color{ 238, 238, 210, 255 };

    DrawRectangleRounded(overlayYesButton, buttonRoundness, buttonSegments, yesColor);
    DrawRectangleRounded(overlayNoButton, buttonRoundness, buttonSegments, noColor);

    DrawTextEx(uiFont22, "Yes",
        {
            std::round(overlayYesButton.x + (overlayYesButton.width - yesSize.x) * 0.5f),
            std::round(overlayYesButton.y + (overlayYesButton.height - yesSize.y) * 0.5f)
        },
        bodyFontSize, 1, BLACK);

    DrawTextEx(uiFont22, "No",
        {
            std::round(overlayNoButton.x + (overlayNoButton.width - noSize.x) * 0.5f),
            std::round(overlayNoButton.y + (overlayNoButton.height - noSize.y) * 0.5f)
        },
        bodyFontSize, 1, BLACK);
}

void Draw::infoOverlay(const char* messageText) {
    const float overlayWidth = 400.0f;
    const float overlayHeight = 150.0f;
    Rectangle overlayRect = {
        ((float)(WIDTH + PANEL_WIDTH) - overlayWidth) * 0.5f,
        ((float)LENGTH - overlayHeight) * 0.5f,
        overlayWidth,
        overlayHeight
    };

    DrawRectangle(0, 0, WIDTH + PANEL_WIDTH, LENGTH, Fade(BLACK, 0.25f));
    DrawRectangleRec(overlayRect, RAYWHITE);
    DrawRectangleLinesEx(overlayRect, 2, GRAY);

    const Vector2 messageSize = MeasureTextEx(uiFont22, messageText, 22, 1);
    DrawTextEx(uiFont22, messageText,
        {
            std::round(overlayRect.x + (overlayRect.width - messageSize.x) * 0.5f),
            std::round(overlayRect.y + (overlayRect.height - messageSize.y) * 0.5f)
        },
        22, 1, DARKGRAY);
}

void Draw::renderOverlay(OverlayType overlayType, float buttonRoundness, int buttonSegments, const std::vector<std::string>& moveHistory, const std::string& winnerName, const std::string& winReason, const std::string& infoMessage) {
    if (overlayType == OverlayType::None) return;

    if (overlayType == OverlayType::Results) {
        showResults(winnerName, winReason, moveHistory.size(), moveHistory);
        return;
    }

    if (overlayType == OverlayType::MoveHistory) {
        showMoveHistory(moveHistory);
        return;
    }

    if (overlayType == OverlayType::Info) {
        infoOverlay(infoMessage.c_str());
        return;
    }

    const char* messageText = "";

    switch (overlayType) {
        case OverlayType::Resign:
            messageText = "Are you sure you want to resign?";
            break;
        case OverlayType::Draw:
            messageText = "Are you sure you want to offer a draw?";
            break;
        default:
            return;
    }

    confirmationOverlay(
        overlayRect,
        overlayYesButton,
        overlayNoButton,
        messageText,
        input.isOverlayYesClicked(),
        input.isOverlayNoClicked(),
        buttonRoundness,
        buttonSegments
    );
}

void Draw::mainMenu(bool playerPlaysWhite, int stockfishElo) {
    float centerX = (WIDTH + PANEL_WIDTH) / 2.0f;
    float centerY = LENGTH / 2.0f;

    // Tiile
    const std::string titleText = "Chess";
    Vector2 titleSize = MeasureTextEx(uiFont36, titleText.c_str(), 36, 1);
    const float titleX = centerX - titleSize.x / 2.0f;
    const float titleY = centerY / 2.0f - titleSize.y / 2.0f;

    // Start button
    const std::string startBtnText = "Start game";;
    Vector2 startBtnSize = MeasureTextEx(uiFont22, startBtnText.c_str(), 22, 1);
    const float startBtnX = centerX - startBtnSize.x / 2.0f - 20.0f;
    const float startBtnY = titleY + titleSize.y + 40.0f;
    startGameButton = Rectangle{
        roundf(startBtnX),
        roundf(startBtnY),
        startBtnSize.x + 40.0f,
        startBtnSize.y + 20.0f
    };

    // Color selector
    const std::string colorSelectorTitle = "Play as";
    const std::string whiteOption = "White";
    const std::string blackOption = "Black";
    Vector2 colorSelectorTitleSize = MeasureTextEx(uiFont22, colorSelectorTitle.c_str(), 22, 1);
    Vector2 whiteOptionSize = MeasureTextEx(uiFont22, whiteOption.c_str(), 22, 1);
    Vector2 blackOptionSize = MeasureTextEx(uiFont22, blackOption.c_str(), 22, 1);
    const float colorSelectorTitleX = centerX - colorSelectorTitleSize.x / 2.0f;
    const float colorSelectorTitleY = startBtnY + startBtnSize.y + 40.0f;
    const float whiteOptionX = centerX - whiteOptionSize.x - 20.0f;
    const float whiteOptionY = colorSelectorTitleY + colorSelectorTitleSize.y + 20.0f;
    const float blackOptionX = centerX + 20.0f;
    const float blackOptionY = whiteOptionY;
    selectWhiteButton = Rectangle{
        roundf(whiteOptionX - 10.0f),
        roundf(whiteOptionY - 5.0f),
        whiteOptionSize.x + 20.0f,
        whiteOptionSize.y + 10.0f
    };
    selectBlackButton = Rectangle{
        roundf(blackOptionX - 10.0f),
        roundf(blackOptionY - 5.0f),
        blackOptionSize.x + 20.0f,
        blackOptionSize.y + 10.0f
    };

    // Elo selector
    const std::string eloSelectorTitle = "Stockfish Elo";
    const std::string eloSelectorValue = "<  " + std::to_string(stockfishElo) + "  >";
    const std::string eloSelectorHint = "Click to change";
    Vector2 eloSelectorTitleSize = MeasureTextEx(uiFont22, eloSelectorTitle.c_str(), 22, 1);
    Vector2 eloSelectorValueSize = MeasureTextEx(uiFont22, eloSelectorValue.c_str(), 22, 1);
    Vector2 eloSelectorHintSize = MeasureTextEx(uiFont22, eloSelectorHint.c_str(), 22, 1);
    const float eloSelectorTitleX = centerX - eloSelectorTitleSize.x / 2.0f;
    const float eloSelectorTitleY = whiteOptionY + whiteOptionSize.y + 36.0f;
    const float eloSelectorValueX = centerX - eloSelectorValueSize.x / 2.0f;
    const float eloSelectorValueY = eloSelectorTitleY + eloSelectorTitleSize.y + 18.0f;
    const float eloSelectorHintX = centerX - eloSelectorHintSize.x / 2.0f;
    const float eloSelectorHintY = eloSelectorValueY + eloSelectorValueSize.y + 14.0f;
    selectEloButton = Rectangle{
        roundf(eloSelectorValueX - 18.0f),
        roundf(eloSelectorValueY - 10.0f),
        eloSelectorValueSize.x + 36.0f,
        eloSelectorValueSize.y + 20.0f
    };

    const std::string exitBtnText = "Exit";
    Vector2 exitBtnSize = MeasureTextEx(uiFont22, exitBtnText.c_str(), 22, 1);
    const float exitBtnX = centerX - exitBtnSize.x / 2.0f - 20.0f;
    const float exitBtnY = startBtnY + startGameButton.height + 240.0f;
    exitButton = Rectangle{
        roundf(exitBtnX),
        roundf(exitBtnY),
        exitBtnSize.x + 40.0f,
        exitBtnSize.y + 20.0f
    };

    // Draw
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawTextEx(uiFont36, titleText.c_str(),
        {
            roundf(titleX),
            roundf(titleY)
        },
        36, 1, BLACK);

    DrawRectangleRounded(startGameButton, 0.2f, 8, input.isStartGameClicked() ? GRAY : LIGHTGRAY);
    DrawTextEx(uiFont22, startBtnText.c_str(),
        {
            roundf(startBtnX + (startGameButton.width - startBtnSize.x) / 2.0f),
            roundf(startBtnY + 10.0f)
        },
        22, 1, BLACK);

    
    DrawTextEx(uiFont22, colorSelectorTitle.c_str(),
        {
            roundf(colorSelectorTitleX),
            roundf(colorSelectorTitleY)
        },
        22, 1, BLACK);

    DrawRectangleRounded(selectWhiteButton, 0.2f, 8, input.isSelectWhiteClicked() ? GRAY : LIGHTGRAY);
    if (playerPlaysWhite) {
        DrawRectangleLinesEx(selectWhiteButton, 3, BLACK);
    }
    DrawTextEx(uiFont22, whiteOption.c_str(),
        {
            roundf(whiteOptionX),
            roundf(whiteOptionY)
        },
        22, 1, BLACK);  

    DrawRectangleRounded(selectBlackButton, 0.2f, 8, input.isSelectBlackClicked() ? GRAY : LIGHTGRAY);
    if (!playerPlaysWhite) {
        DrawRectangleLinesEx(selectBlackButton, 3, BLACK);
    }
    DrawTextEx(uiFont22, blackOption.c_str(),
        {
            roundf(blackOptionX),
            roundf(blackOptionY)
        },
        22, 1, BLACK);

    DrawTextEx(uiFont22, eloSelectorTitle.c_str(),
        {
            roundf(eloSelectorTitleX),
            roundf(eloSelectorTitleY)
        },
        22, 1, BLACK);

    Color eloButtonColor = input.isSelectEloClicked() ? GRAY : LIGHTGRAY;
    DrawRectangleRounded(selectEloButton, 0.2f, 8, eloButtonColor);
    DrawTextEx(uiFont22, eloSelectorValue.c_str(),
        {
            roundf(eloSelectorValueX),
            roundf(eloSelectorValueY)
        },
        22, 1, BLACK);

    DrawTextEx(uiFont22, eloSelectorHint.c_str(),
        {
            roundf(eloSelectorHintX),
            roundf(eloSelectorHintY)
        },
        22, 1, GRAY);

    DrawRectangleRounded(exitButton, 0.2f, 8, input.isExitClicked() ? GRAY : LIGHTGRAY);
    DrawTextEx(uiFont22, exitBtnText.c_str(),
        {
            roundf(exitBtnX + (exitButton.width - exitBtnSize.x) / 2.0f),
            roundf(exitBtnY + 10.0f)
        },
        22, 1, BLACK);

    EndDrawing();
}

void Draw::render(Board& board, bool pieceSelected, int selectedX, int selectedY, std::set<std::pair<int, int>>& possibleMoves, std::set<std::pair<int, int>>& possibleCaptures, Move* lastMove, bool hasLastMove, OverlayType overlayType, std::string playerName, bool playerPlaysWhite, float playerTimeLeftSeconds, const std::vector<std::string>& moveHistory, const std::string& winnerName, const std::string& winReason, const std::string& infoMessage) {
    static float PIECE_SCALE = 0.46f;
    static float PLAYER_SCALE = 0.28f;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    float boardScale = (float)WIDTH / (float)boardTexture.width;
    DrawTextureEx(boardTexture, { 0.0f, 0.0f }, 0.0f, boardScale, WHITE);

    float cellSize = WIDTH / 8.0f;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int drawBoardX = playerPlaysWhite ? x : 7 - x;
            int drawBoardY = playerPlaysWhite ? y : 7 - y;

            Color hightlight;
            if((x + y) % 2) {
                hightlight = Color {185, 202, 67, 255};
            } else {
                hightlight = Color {245, 246, 130, 255};
            }

            if(hasLastMove && lastMove->getFromX() == x && lastMove->getFromY() == y) {
                DrawRectangle((int)drawBoardX * cellSize, (int)drawBoardY * cellSize, cellSize + 1, cellSize + 1, hightlight);
            }

            if(hasLastMove && lastMove->getToX() == x && lastMove->getToY() == y) {
                DrawRectangle((int)drawBoardX * cellSize, (int)drawBoardY * cellSize, cellSize + 1, cellSize + 1, hightlight);
            }

            float centerX = drawBoardX * cellSize + cellSize * 0.5f;
            float centerY = drawBoardY * cellSize + cellSize * 0.5f;

            if(possibleMoves.find(std::make_pair(x, y)) != possibleMoves.end()) {
                DrawCircle(
                    (int)centerX,
                    (int)centerY,
                    cellSize * 0.18f,
                    Fade(DARKGRAY, 0.5f)
                );
            }

            if(possibleCaptures.find(std::make_pair(x, y)) != possibleCaptures.end()) {
                float radius = cellSize * 0.4f;
                float thickness = cellSize * 0.08f;

                DrawRing(
                    (Vector2){centerX, centerY},
                    radius - thickness,
                    radius,
                    0.0f,
                    360.0f,
                    32,
                    Fade(BLACK, 0.5f)
                );
            }

            Piece* piece = board.getPiece(x, y);
            if (piece == nullptr) continue;

            if (pieceSelected && x == selectedX && y == selectedY) {
                DrawRectangle(
                    (int)drawBoardX * cellSize,
                    (int)drawBoardY * cellSize,
                    cellSize + 1, // Need to add plus one to width because of rounding above makes small hole
                    cellSize + 1,
                    hightlight
                );
            }

            Texture2D* tex = nullptr;

            if (piece->getColor() == PieceColor::Black) {
                switch (piece->getType()) {
                    case PieceType::Pawn:   tex = &b_pawn;   break;
                    case PieceType::Knight: tex = &b_knight; break;
                    case PieceType::Bishop: tex = &b_bishop; break;
                    case PieceType::Rook:   tex = &b_rook;   break;
                    case PieceType::Queen:  tex = &b_queen;  break;
                    case PieceType::King:   tex = &b_king;   break;
                }
            } else {
                switch (piece->getType()) {
                    case PieceType::Pawn:   tex = &w_pawn;   break;
                    case PieceType::Knight: tex = &w_knight; break;
                    case PieceType::Bishop: tex = &w_bishop; break;
                    case PieceType::Rook:   tex = &w_rook;   break;
                    case PieceType::Queen:  tex = &w_queen;  break;
                    case PieceType::King:   tex = &w_king;   break;
                }
            }

            if (tex == nullptr) continue;

            float drawW = (float)tex->width * PIECE_SCALE;
            float drawH = (float)tex->height * PIECE_SCALE;

            float drawX = drawBoardX * cellSize + (cellSize - drawW) * 0.5f;
            float drawY = drawBoardY * cellSize + (cellSize - drawH) * 0.5f;

            DrawTextureEx(*tex, { drawX, drawY }, 0.0f, PIECE_SCALE, WHITE);
        }
    }
    const float playerImageTop       = 40.0f;
    const float playerNameSize       = 36.0f;
    const float playerNameSpacingY   = 14.0f;
    const float panelSidePadding     = 18.0f;
    const float buttonsTopSpacing    = 28.0f;
    const float buttonHeight         = 48.0f;
    const float buttonSpacing        = 18.0f;
    const float buttonSize           = 22.0f;
    const float buttonRoundness      = 0.2f;
    const int   buttonSegments       = 8;

    DrawRectangle(WIDTH, 0, PANEL_WIDTH, LENGTH, Color{ 117, 148, 85, 255 });
    DrawLine(WIDTH, 0, WIDTH, LENGTH, Color{ 238, 238, 210, 255 });

    float playerImageW = (float)stockfishTexture.width * PLAYER_SCALE;
    float playerImageH = (float)stockfishTexture.height * PLAYER_SCALE;
    float playerImageX = WIDTH + (PANEL_WIDTH - playerImageW) * 0.5f;
    float playerImageY = playerImageTop;

    DrawTextureEx(stockfishTexture, { playerImageX, playerImageY }, 0.0f, PLAYER_SCALE, WHITE);

    Vector2 playerTextSize = MeasureTextEx(uiFont36, playerName.c_str(), 36, 1);
    float playerTextX = WIDTH + (PANEL_WIDTH - playerTextSize.x) * 0.5f;
    float playerTextY = playerImageY + playerImageH + playerNameSpacingY;

    DrawTextEx(uiFont36, playerName.c_str(),
    {
        (float)std::round(playerTextX),
        (float)std::round(playerTextY)
    },
    36, 1, Color{ 238, 238, 210, 255 });

    float buttonWidth = PANEL_WIDTH - panelSidePadding * 2.0f;
    float buttonX = WIDTH + (PANEL_WIDTH - buttonWidth) * 0.5f;
    float firstButtonY = playerTextY + playerTextSize.y + buttonsTopSpacing;

    resignButton = { buttonX, firstButtonY, buttonWidth, buttonHeight };
    offerDrawButton = { buttonX, firstButtonY + buttonHeight + buttonSpacing, buttonWidth, buttonHeight };
    showResultsButton = { buttonX, firstButtonY + (buttonHeight + buttonSpacing) * 2.0f, buttonWidth, buttonHeight };
    undoButton = {buttonX,firstButtonY + (buttonHeight + buttonSpacing) * 3.0f,buttonWidth,buttonHeight};

Color resignColor = input.isResignClicked()
    ? Color{ 218, 218, 190, 255 }
    : Color{ 238, 238, 210, 255 };

Color drawColor = input.isOfferDrawClicked()
    ? Color{ 218, 218, 190, 255 }
    : Color{ 238, 238, 210, 255 };

Color resultsColor = input.isShowResultsClicked()
    ? Color{ 218, 218, 190, 255 }
    : Color{ 238, 238, 210, 255 };
    Color undoColor = input.isUndoClicked() ? GRAY : LIGHTGRAY;

    DrawRectangleRounded(resignButton, buttonRoundness, buttonSegments, resignColor);
    DrawRectangleRounded(offerDrawButton, buttonRoundness, buttonSegments, drawColor);
    DrawRectangleRounded(showResultsButton, buttonRoundness, buttonSegments, resultsColor);
    DrawRectangleRounded(undoButton, buttonRoundness, buttonSegments, undoColor);

    const char* resignText = "Resign";
    const char* drawTextStr = "Offer Draw";
    const char* resultsText = "Show Moves";
    const char* undoText = "Undo Move";

    Vector2 resignSize = MeasureTextEx(uiFont22, resignText, 22, 1);
    Vector2 drawSize = MeasureTextEx(uiFont22, drawTextStr, 22, 1);
    Vector2 resultsSize = MeasureTextEx(uiFont22, resultsText, 22, 1);
    Vector2 undoSize = MeasureTextEx(uiFont22, undoText, 22, 1);

    DrawTextEx(uiFont22, resignText,
        {
            roundf(resignButton.x + (resignButton.width - resignSize.x) * 0.5f),
            roundf(resignButton.y + (resignButton.height - resignSize.y) * 0.5f)
        },
        22, 1, BLACK);

    DrawTextEx(uiFont22, drawTextStr,
        {
            roundf(offerDrawButton.x + (offerDrawButton.width - drawSize.x) * 0.5f),
            roundf(offerDrawButton.y + (offerDrawButton.height - drawSize.y) * 0.5f)
        },
        22, 1, BLACK);

    DrawTextEx(uiFont22, resultsText,
        {
            roundf(showResultsButton.x + (showResultsButton.width - resultsSize.x) * 0.5f),
            roundf(showResultsButton.y + (showResultsButton.height - resultsSize.y) * 0.5f)
        },
        22, 1, BLACK);
    DrawTextEx(uiFont22, undoText,
    {
        roundf(undoButton.x + (undoButton.width - undoSize.x) * 0.5f),
        roundf(undoButton.y + (undoButton.height - undoSize.y) * 0.5f)
    },
    22, 1, BLACK);

    const float playerClockBottomPadding = 18.0f;
    const float playerClockHeight = 64.0f;
    Rectangle playerClockRect = {
        buttonX,
        LENGTH - playerClockBottomPadding - playerClockHeight,
        buttonWidth,
        playerClockHeight
    };

    std::string playerClockText = formatClock(playerTimeLeftSeconds);
    Vector2 playerClockSize = MeasureTextEx(uiFont36, playerClockText.c_str(), 36, 1);
    DrawRectangleRounded(playerClockRect, buttonRoundness, buttonSegments, Color{ 238, 238, 210, 255 });
    DrawTextEx(uiFont36, playerClockText.c_str(),
    {
        roundf(playerClockRect.x + (playerClockRect.width - playerClockSize.x) * 0.5f),
        roundf(playerClockRect.y + (playerClockRect.height - playerClockSize.y) * 0.5f)
    },
    36, 1, BLACK);

    if (overlayType != OverlayType::None) {
        renderOverlay(overlayType, buttonRoundness, buttonSegments, moveHistory, winnerName, winReason, infoMessage);
    }

    EndDrawing();
}
