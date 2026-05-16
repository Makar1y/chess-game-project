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


void DrawBackgroundCover(Texture2D texture, Rectangle destRect) {
    float bgAspect = (float)texture.width / (float)texture.height;
    float destAspect = destRect.width / destRect.height;
    
    Rectangle sourceRect;
    
    if (bgAspect > destAspect) {
        // Background wider
        float scale = destRect.height / (float)texture.height;
        float scaledWidth = (float)texture.width * scale;
        float cropX = (scaledWidth - destRect.width) / 2.0f / scale;
        
        sourceRect = Rectangle{
            cropX, 0.0f,
            (float)texture.width - cropX * 2.0f,
            (float)texture.height
        };
    } else {
        // Background taller
        float scale = destRect.width / (float)texture.width;
        float scaledHeight = (float)texture.height * scale;
        float cropY = (scaledHeight - destRect.height) / 2.0f / scale;
        
        sourceRect = Rectangle{
            0.0f, cropY,
            (float)texture.width,
            (float)texture.height - cropY * 2.0f
        };
    }
    
    DrawTexturePro(texture, sourceRect, destRect, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
}

Draw::Draw()
    : input(resignButton, offerDrawButton, showResultsButton, overlayYesButton, overlayNoButton,
            selectWhiteButton, selectBlackButton, startGameButton, selectEloButton,
            selectEloLeftButton, selectEloRightButton, selectTimeButton, selectTimeLeftButton,
            selectTimeRightButton, undoButton, backToGameButton, exitToMenuButton,
            newGameButton, exitButton, pvpMenuButton, hostGameButton, joinGameButton,
            ipInputBox, portInputBox, backButton, promoteQueenSelection, promoteKnightSelection,
            promoteRookSelection, promoteBishopSelection, cancelPromotionButton) {
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
    pvpMenuButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    hostGameButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    joinGameButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    ipInputBox = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    portInputBox = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    backButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    promoteQueenSelection = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    promoteKnightSelection = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    promoteRookSelection = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    promoteBishopSelection = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    cancelPromotionButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };


    // main menu buttons
    startGameButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectWhiteButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectBlackButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectEloButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectEloLeftButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectEloRightButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectTimeButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectTimeLeftButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectTimeRightButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };

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

    historyScrollOffset = 0.0f;
}

void Draw::loadResources() {
    if (resourcesLoaded) return;

    uiFont22 = LoadFontEx("Fonts/Rubik-SemiBold.ttf", 22, 0, 0);
    uiFont36 = LoadFontEx("Fonts/Rubik-SemiBold.ttf", 36, 0, 0);

    SetTextureFilter(uiFont22.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(uiFont36.texture, TEXTURE_FILTER_BILINEAR);

    backgroundTexture = LoadTexture("Textures/background.png");
    boardTexture = LoadTexture("Textures/board.png");
    stockfishTexture = LoadTexture("Textures/stockfish.png");
    guestTexture = LoadTexture("Textures/guest.png");

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

    SetTextureFilter(backgroundTexture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(stockfishTexture, TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(guestTexture, TEXTURE_FILTER_TRILINEAR);

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

    UnloadTexture(backgroundTexture);
    UnloadTexture(boardTexture);
    UnloadTexture(stockfishTexture);
    UnloadTexture(guestTexture);

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

void Draw::resetHistoryScroll() {
    historyScrollOffset = 0.0f;
}

void Draw::renderButton(const Rectangle& rect, const char* text, bool hovered, bool selected, Color baseColor, Color textColor, Color borderColor, float fontSize) {
    Color btnColor = hovered ? (baseColor.r + baseColor.g + baseColor.b > 600 ? GRAY : DARKGRAY) : baseColor;
    if (baseColor.r == RED.r && baseColor.g == RED.g && baseColor.b == RED.b) {
        btnColor = hovered ? Color{185, 41, 55, 255} : RED;
    }

    DrawRectangleRounded(rect, 0.3f, 8, btnColor);
    if (selected) {
        DrawRectangleLinesEx(rect, 4, YELLOW);
    } else {
        DrawRectangleLinesEx(rect, 2, borderColor);
    }

    Vector2 textSize = MeasureTextEx(uiFont22, text, fontSize, 1);
    DrawTextEx(uiFont22, text, {
        roundf(rect.x + (rect.width - textSize.x) * 0.5f),
        roundf(rect.y + (rect.height - textSize.y) * 0.5f)
    }, fontSize, 1, textColor);
}

void Draw::showMoveHistory(const std::vector<std::string>& moveHistory) {
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

    std::string movesStr = "Total Moves: " + std::to_string(moveHistory.size());
    DrawTextEx(uiFont22, movesStr.c_str(), { historyOverlayRect.x + 20, historyOverlayRect.y + 40 }, 18, 1, DARKGRAY);

    std::string historyText = "";
    for (size_t i = 0; i < moveHistory.size(); ++i) {
        if (i % 2 == 0) {
            historyText += std::to_string(i / 2 + 1) + ". ";
        }
        historyText += moveHistory[i] + " ";
        if (i % 2 != 0) historyText += "\n";
    }

    // Scrolling logic
    float wheel = GetMouseWheelMove();
    historyScrollOffset += wheel * 25.0f;
    
    Vector2 textSize = MeasureTextEx(uiFont22, historyText.c_str(), 18, 1);
    float visibleHeight = historyOverlayRect.height - 150.0f; // Space between header and buttons
    float maxScroll = textSize.y - visibleHeight;
    if (maxScroll < 0) maxScroll = 0;
    
    if (historyScrollOffset > 0) historyScrollOffset = 0;
    if (historyScrollOffset < -maxScroll) historyScrollOffset = -maxScroll;

    // Draw move history with clipping
    BeginScissorMode((int)historyOverlayRect.x, (int)(historyOverlayRect.y + 70), (int)historyOverlayRect.width, (int)visibleHeight);
    DrawTextEx(uiFont22, historyText.c_str(), { historyOverlayRect.x + 20, historyOverlayRect.y + 70 + historyScrollOffset }, 18, 1, BLACK);
    EndScissorMode();

    // Draw scroll indicator
    if (maxScroll > 0) {
        float scrollBarHeight = (visibleHeight / textSize.y) * visibleHeight;
        float scrollBarY = historyOverlayRect.y + 70 + (-historyScrollOffset / maxScroll) * (visibleHeight - scrollBarHeight);
        DrawRectangle(historyOverlayRect.x + historyOverlayRect.width - 10, scrollBarY, 5, scrollBarHeight, Fade(GRAY, 0.6f));
    }

    // Draw buttons
    renderButton(backToGameButton, "Back to Game", input.isBackToGameClicked(), false, Color{ 238, 238, 210, 255 }, BLACK, BLACK, 18.0f);
}

void Draw::showResults(const std::string& winnerName, const std::string& winReason, int movesMade, const std::vector<std::string>& moveHistory) {
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

    const char* historyTitle = "Move History:";
    DrawTextEx(uiFont22, historyTitle, { resultsOverlayRect.x + 20, resultsOverlayRect.y + 145 }, 18, 1, DARKGRAY);

    std::string historyText = "";
    for (size_t i = 0; i < moveHistory.size(); ++i) {
        if (i % 2 == 0) {
            historyText += std::to_string(i / 2 + 1) + ". ";
        }
        historyText += moveHistory[i] + " ";
        if (i % 2 != 0) historyText += "\n";
    }

    // Scrolling logic
    float wheel = GetMouseWheelMove();
    historyScrollOffset += wheel * 25.0f;

    Vector2 textSize = MeasureTextEx(uiFont22, historyText.c_str(), 18, 1);
    float visibleHeight = resultsOverlayRect.height - 175.0f - 80.0f; // From Y=175 to buttons
    float maxScroll = textSize.y - visibleHeight;
    if (maxScroll < 0) maxScroll = 0;

    if (historyScrollOffset > 0) historyScrollOffset = 0;
    if (historyScrollOffset < -maxScroll) historyScrollOffset = -maxScroll;

    // Draw move history with clipping
    BeginScissorMode((int)resultsOverlayRect.x, (int)(resultsOverlayRect.y + 175), (int)resultsOverlayRect.width, (int)visibleHeight);
    DrawTextEx(uiFont22, historyText.c_str(), { resultsOverlayRect.x + 20, resultsOverlayRect.y + 175 + historyScrollOffset }, 18, 1, BLACK);
    EndScissorMode();

    // Scroll indicator
    if (maxScroll > 0) {
        float scrollBarHeight = (visibleHeight / textSize.y) * visibleHeight;
        float scrollBarY = resultsOverlayRect.y + 175 + (-historyScrollOffset / maxScroll) * (visibleHeight - scrollBarHeight);
        DrawRectangle(resultsOverlayRect.x + resultsOverlayRect.width - 10, scrollBarY, 5, scrollBarHeight, Fade(GRAY, 0.6f));
    }

    // Draw buttons
    renderButton(exitToMenuButton, "Exit to Menu", input.isExitToMenuClicked(), false, Color{ 238, 238, 210, 255 }, BLACK, BLACK, 18.0f);
    renderButton(newGameButton, "New Game", input.isNewGameClicked(), false, Color{ 238, 238, 210, 255 }, BLACK, BLACK, 18.0f);
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

    renderButton(overlayYesButton, "Yes", overlayYesPressed, false, Color{ 238, 238, 210, 255 });
    renderButton(overlayNoButton, "No", overlayNoPressed, false, Color{ 238, 238, 210, 255 });
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

void Draw::showPromotion(const std::vector<std::string>& moveHistory) {  
    std::string lastmove = moveHistory.back().substr(2, 2);
    bool playerPlaysWhite = lastmove[1] == '8' ? true : false;
    int x = lastmove[0] - 'a';
    int y = '8' - lastmove[1];
    int drawBoardX = playerPlaysWhite ? x : 7 - x;
    int drawBoardY = playerPlaysWhite ? y : 7 - y;
    float PIECE_SCALE = 0.46f;
    float cellSize = WIDTH / 8.0f;

    Texture2D* tex[4];
    if(playerPlaysWhite)
    {
        tex[0] = &w_queen;
        tex[1] = &w_knight;
        tex[2] = &w_rook;
        tex[3] = &w_bishop;
    }
    else
    {
        tex[0] = &b_queen;
        tex[1] = &b_knight;
        tex[2] = &b_rook;
        tex[3] = &b_bishop;
    }

    promoteQueenSelection   = {(float)drawBoardX * cellSize, (float)drawBoardY * cellSize, cellSize, cellSize};
    promoteKnightSelection  = {(float)drawBoardX * cellSize, (float)(drawBoardY + 1) * cellSize, cellSize, cellSize};
    promoteRookSelection    = {(float)drawBoardX * cellSize, (float)(drawBoardY + 2) * cellSize, cellSize, cellSize};
    promoteBishopSelection  = {(float)drawBoardX * cellSize, (float)(drawBoardY + 3) * cellSize, cellSize, cellSize};

    Rectangle buttons[4] = {
        promoteQueenSelection,
        promoteKnightSelection,
        promoteRookSelection,
        promoteBishopSelection
    };

    for(int i = 0; i < 4; i++)
    {
        DrawRectangleRec(buttons[i], WHITE);

        float drawW = (float)tex[i]->width * PIECE_SCALE;
        float drawH = (float)tex[i]->height * PIECE_SCALE;

        float drawX = buttons[i].x + (cellSize - drawW) * 0.5f;
        float drawY = buttons[i].y + (cellSize - drawH) * 0.5f;

        DrawTextureEx(*tex[i], { drawX, drawY }, 0.0f, PIECE_SCALE, WHITE);

        if(CheckCollisionPointRec(GetMousePosition(), buttons[i]))
        {
            DrawRectangleLinesEx(buttons[i], 3, GOLD);
        }
    }

    float posX = drawBoardX * cellSize;
    float posY = (drawBoardY + 4) * cellSize;

    Color bgColor = {241, 241, 241, 255};
    Color xColor = {139, 137, 135, 255};

    float cancelH = cellSize / 2.0f;

    cancelPromotionButton = {
        posX,
        posY,
        cellSize,
        cancelH
    };

    DrawRectangleRec(cancelPromotionButton, bgColor);

    if(CheckCollisionPointRec(GetMousePosition(), cancelPromotionButton))
    {
        DrawRectangleLinesEx(cancelPromotionButton, 3, LIGHTGRAY);
    }

    const char* text = "x";
    int fontSize = 30;

    int textWidth = MeasureText(text, fontSize);

    float textX = posX + (cellSize - textWidth) / 2;
    float textY = posY + (cancelH - fontSize) / 2;

    DrawText(text, textX,     textY,     fontSize, xColor);
    DrawText(text, textX + 1, textY,     fontSize, xColor);
    DrawText(text, textX,     textY + 1, fontSize, xColor);
    DrawText(text, textX + 1, textY + 1, fontSize, xColor);
}

void Draw::renderOverlay(OverlayType overlayType, float buttonRoundness, int buttonSegments, const std::vector<std::string>& moveHistory, const std::string& winnerName, const std::string& winReason, const std::string& infoMessage) {
    if (overlayType == OverlayType::None) return;

    if (overlayType == OverlayType::Results) {
        showResults(winnerName, winReason, moveHistory.size(), moveHistory);
        return;
    }

    if(overlayType == OverlayType::Promotion) {
        showPromotion(moveHistory);
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
        case OverlayType::DrawOfferReceived:
            messageText = "Opponent offers a draw. Accept?";
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

void Draw::mainMenu(bool playerPlaysWhite, const std::string& difficultyName, float timeControlSeconds) {
    float centerX = (WIDTH + PANEL_WIDTH) / 2.0f;
    float centerY = LENGTH / 2.0f;

    // Constants for layout
    const float titleY = 60.0f;
    const float buttonGapHalf = 10.0f;         // Half of horizontal gap between paired buttons
    const float titleToButtonOffset = 28.0f;   // Vertical distance from title text to buttons
    const float sectionSpacing = 35.0f;        // Vertical distance between UI sections
    const float internalPaddingX = 20.0f;      // Horizontal padding inside buttons
    const float internalPaddingY = 16.0f;      // Vertical padding inside buttons
    const float arrowGap = 5.0f;               // Gap between selector arrows and value
    const float buttonVerticalGap = 15.0f;     // Gap between vertical buttons
    const float arrowWidth = 40.0f;

    // Calculations for all UI elements
    auto getBtnRect = [&](const std::string& text, float y, float pX = 40.0f, float pY = 20.0f) {
        Vector2 size = MeasureTextEx(uiFont22, text.c_str(), 22, 1);
        return Rectangle{ roundf(centerX - (size.x + pX) / 2.0f), roundf(y), size.x + pX, size.y + pY };
    };
    
    // Define rectangles
    startGameButton = getBtnRect("PvE (Stockfish)", titleY + 120.0f);
    pvpMenuButton = getBtnRect("PvP Match", startGameButton.y + startGameButton.height + buttonVerticalGap);

    float colorY = pvpMenuButton.y + pvpMenuButton.height + sectionSpacing;
    Vector2 whiteSize = MeasureTextEx(uiFont22, "White", 22, 1);
    Vector2 blackSize = MeasureTextEx(uiFont22, "Black", 22, 1);
    selectWhiteButton = { roundf(centerX - whiteSize.x - internalPaddingX - buttonGapHalf), roundf(colorY + titleToButtonOffset), whiteSize.x + internalPaddingX, whiteSize.y + internalPaddingY };
    selectBlackButton = { roundf(centerX + buttonGapHalf), roundf(colorY + titleToButtonOffset), blackSize.x + internalPaddingX, blackSize.y + internalPaddingY };

    float eloY = selectWhiteButton.y + selectWhiteButton.height + sectionSpacing;
    const std::string eloValueText = difficultyName;
    Vector2 eloValueSize = MeasureTextEx(uiFont22, eloValueText.c_str(), 22, 1);
    selectEloButton = { roundf(centerX - (eloValueSize.x + 50.0f) / 2.0f), roundf(eloY + titleToButtonOffset), eloValueSize.x + 50.0f, eloValueSize.y + 24.0f };
    selectEloLeftButton = { roundf(selectEloButton.x - arrowWidth - arrowGap), selectEloButton.y, arrowWidth, selectEloButton.height };
    selectEloRightButton = { roundf(selectEloButton.x + selectEloButton.width + arrowGap), selectEloButton.y, arrowWidth, selectEloButton.height };

    float timeY = selectEloButton.y + selectEloButton.height + sectionSpacing;
    const std::string timeValueText = formatClock(timeControlSeconds);
    Vector2 timeValueSize = MeasureTextEx(uiFont22, timeValueText.c_str(), 22, 1);
    selectTimeButton = { roundf(centerX - (timeValueSize.x + 50.0f) / 2.0f), roundf(timeY + titleToButtonOffset), timeValueSize.x + 50.0f, timeValueSize.y + 24.0f };
    selectTimeLeftButton = { roundf(selectTimeButton.x - arrowWidth - arrowGap), selectTimeButton.y, arrowWidth, selectTimeButton.height };
    selectTimeRightButton = { roundf(selectTimeButton.x + selectTimeButton.width + arrowGap), selectTimeButton.y, arrowWidth, selectTimeButton.height };

    exitButton = getBtnRect("Exit", LENGTH - 80.0f);

    BeginDrawing();
    DrawBackgroundCover(backgroundTexture, {0, 0, (float)WIDTH + PANEL_WIDTH, (float)LENGTH});

    // Title
    Vector2 titleSize = MeasureTextEx(uiFont36, "Chess", 36, 1);
    DrawTextEx(uiFont36, "Chess", { roundf(centerX - titleSize.x / 2.0f + 2), titleY + 2 }, 36, 1, Color{0,0,0,100});
    DrawTextEx(uiFont36, "Chess", { roundf(centerX - titleSize.x / 2.0f), titleY }, 36, 1, WHITE);

    // Render Buttons
    renderButton(startGameButton, "PvE (Stockfish)", input.isStartGameClicked());
    renderButton(pvpMenuButton, "PvP Match", input.isPvpMenuClicked());

    // Color Selector
    DrawTextEx(uiFont22, "Play as:", { roundf(centerX - MeasureTextEx(uiFont22, "Play as:", 22, 1).x / 2.0f), colorY }, 22, 1, WHITE);
    renderButton(selectWhiteButton, "White", input.isSelectWhiteClicked(), playerPlaysWhite, WHITE, BLACK, BLACK);
    renderButton(selectBlackButton, "Black", input.isSelectBlackClicked(), !playerPlaysWhite, BLACK, WHITE, WHITE);

    // Difficulty Selector
    DrawTextEx(uiFont22, "Difficulty:", { roundf(centerX - MeasureTextEx(uiFont22, "Difficulty:", 22, 1).x / 2.0f), eloY }, 22, 1, WHITE);
    renderButton(selectEloLeftButton, "<<", input.isSelectEloLeftClicked());
    renderButton(selectEloButton, eloValueText.c_str(), false);
    renderButton(selectEloRightButton, ">>", input.isSelectEloRightClicked());

    // Time Selector
    DrawTextEx(uiFont22, "Time Control:", { roundf(centerX - MeasureTextEx(uiFont22, "Time Control:", 22, 1).x / 2.0f), timeY }, 22, 1, WHITE);
    renderButton(selectTimeLeftButton, "<<", input.isSelectTimeLeftClicked());
    renderButton(selectTimeButton, timeValueText.c_str(), false);
    renderButton(selectTimeRightButton, ">>", input.isSelectTimeRightClicked());

    renderButton(exitButton, "Exit", input.isExitClicked(), false, RED, WHITE);

    EndDrawing();
}

void Draw::pvpMenu(const std::string& ipText, const std::string& portText, const std::string& statusText, bool ipFocused, bool portFocused) {
    float centerX = (WIDTH + PANEL_WIDTH) / 2.0f;
    const float titleY = 55.0f;
    const float guestScale = 0.22f;

    // Define layouts
    Vector2 titleSize = MeasureTextEx(uiFont36, "PvP Match", 36, 1);
    float guestH = (float)guestTexture.height * guestScale;
    float guestY = titleY + titleSize.y + 24.0f;

    const float fieldWidth = 360.0f;
    const float fieldHeight = 46.0f;
    const float fieldX = centerX - fieldWidth * 0.5f;

    const float titleToButtonOffset = 30.0f;   // Vertical distance from label/title to input/button
    const float sectionSpacing = 35.0f;        // Vertical distance between sections
    const float buttonGapHalf = 5.0f;          // Half of horizontal gap between buttons
    const float buttonWidth = 170.0f;
    const float buttonHeight = 48.0f;

    float ipLabelY = guestY + guestH + sectionSpacing;
    ipInputBox = { fieldX, ipLabelY + titleToButtonOffset, fieldWidth, fieldHeight };

    float portLabelY = ipInputBox.y + ipInputBox.height + 20.0f;
    portInputBox = { fieldX, portLabelY + titleToButtonOffset, fieldWidth, fieldHeight };

    const float buttonsY = portInputBox.y + portInputBox.height + sectionSpacing;
    hostGameButton = { roundf(centerX - buttonWidth - buttonGapHalf), buttonsY, buttonWidth, buttonHeight };
    joinGameButton = { roundf(centerX + buttonGapHalf), buttonsY, buttonWidth, buttonHeight };
    backButton = { roundf(centerX - 75.0f), LENGTH - 80.0f, 150.0f, 48.0f };

    BeginDrawing();
    DrawBackgroundCover(backgroundTexture, {0, 0, (float)WIDTH + PANEL_WIDTH, (float)LENGTH});

    // Title & Texture
    DrawTextEx(uiFont36, "PvP Match", { roundf(centerX - titleSize.x / 2.0f + 2), titleY + 2 }, 36, 1, Color{0, 0, 0, 100});
    DrawTextEx(uiFont36, "PvP Match", { roundf(centerX - titleSize.x / 2.0f), titleY }, 36, 1, WHITE);
    DrawTextureEx(guestTexture, { centerX - (float)guestTexture.width * guestScale * 0.5f, guestY }, 0.0f, guestScale, WHITE);

    // Input Fields
    DrawTextEx(uiFont22, "Host IP:", { fieldX, ipLabelY }, 22, 1, WHITE);
    renderButton(ipInputBox, ipText.c_str(), false, ipFocused, WHITE, BLACK, BLACK);
    
    DrawTextEx(uiFont22, "Port:", { fieldX, portLabelY }, 22, 1, WHITE);
    renderButton(portInputBox, portText.c_str(), false, portFocused, WHITE, BLACK, BLACK);

    // Buttons
    renderButton(hostGameButton, "Create Game", input.isHostGameClicked());
    renderButton(joinGameButton, "Join Game", input.isJoinGameClicked());

    if (!statusText.empty()) {
        Vector2 statusSize = MeasureTextEx(uiFont22, statusText.c_str(), 20, 1);
        DrawTextEx(uiFont22, statusText.c_str(), { centerX - statusSize.x * 0.5f, buttonsY + 72.0f }, 20, 1, WHITE);
    }

    renderButton(backButton, "Back", input.isBackClicked(), false, RED, WHITE);

    EndDrawing();
}

void Draw::connectingMenu(const std::string& statusText) {
    float centerX = (WIDTH + PANEL_WIDTH) / 2.0f;
    float centerY = LENGTH / 2.0f;

    BeginDrawing();
    DrawBackgroundCover(backgroundTexture, Rectangle{0.0f, 0.0f, centerX * 2, centerY * 2});

    const std::string titleText = "pvp Connection";
    Vector2 titleSize = MeasureTextEx(uiFont36, titleText.c_str(), 36, 1);
    DrawTextEx(uiFont36, titleText.c_str(), { centerX - titleSize.x * 0.5f, 120.0f }, 36, 1, WHITE);

    float guestScale = 0.25f;
    DrawTextureEx(guestTexture, { centerX - guestTexture.width * guestScale * 0.5f, 190.0f }, 0.0f, guestScale, WHITE);

    Vector2 statusSize = MeasureTextEx(uiFont22, statusText.c_str(), 22, 1);
    DrawTextEx(uiFont22, statusText.c_str(), { centerX - statusSize.x * 0.5f, 390.0f }, 22, 1, WHITE);

    EndDrawing();
}

void Draw::render(Board& board, bool pieceSelected, int selectedX, int selectedY,
                  std::set<std::pair<int, int>>& possibleMoves, std::set<std::pair<int, int>>& possibleCaptures,
                  Move* lastMove, bool hasLastMove, OverlayType overlayType, std::string playerName,
                  bool playerPlaysWhite, float playerTimeLeftSeconds, const std::vector<std::string>& moveHistory,
                  const std::string& winnerName, const std::string& winReason, const std::string& infoMessage,
                  bool pvpMode, const std::vector<PieceType>& whiteCaptured, const std::vector<PieceType>& blackCaptured) {

    static float PIECE_SCALE = 0.46f;
    static float PLAYER_SCALE = 0.28f;
    static float CAPTURED_PIECE_SCALE = 0.14f;

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
                    cellSize + 1,
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

    Texture2D& playerTexture = pvpMode ? guestTexture : stockfishTexture;
    float playerImageW = (float)playerTexture.width * PLAYER_SCALE;
    float playerImageH = (float)playerTexture.height * PLAYER_SCALE;
    float playerImageX = WIDTH + (PANEL_WIDTH - playerImageW) * 0.5f;
    float playerImageY = playerImageTop;

    DrawTextureEx(playerTexture, { playerImageX, playerImageY }, 0.0f, PLAYER_SCALE, WHITE);

    Vector2 playerTextSize = MeasureTextEx(uiFont36, playerName.c_str(), 36, 1);
    float playerTextX = WIDTH + (PANEL_WIDTH - playerTextSize.x) * 0.5f;
    float playerTextY = playerImageY + playerImageH + playerNameSpacingY;

    DrawTextEx(uiFont36, playerName.c_str(),
    {
        (float)std::round(playerTextX),
        (float)std::round(playerTextY)
    },
    36, 1, Color{ 238, 238, 210, 255 });

    // Captured pieces by opponent
    PieceColor opponentColor = playerPlaysWhite ? PieceColor::Black : PieceColor::White;
    const std::vector<PieceType>& opponentCapturedList = (opponentColor == PieceColor::White) ? whiteCaptured : blackCaptured;
    PieceColor opponentCapturedColor = (opponentColor == PieceColor::White) ? PieceColor::Black : PieceColor::White;

    float capturedX = WIDTH + panelSidePadding;
    float capturedY = playerImageY - 40.0f;
    float capturedSpacing = 22.0f;
    float currentX = capturedX;
    float currentY = capturedY;

    for (PieceType type : opponentCapturedList) {
        Texture2D* tex = nullptr;
        if (opponentCapturedColor == PieceColor::Black) {
            switch (type) {
                case PieceType::Pawn:   tex = &b_pawn;   break;
                case PieceType::Knight: tex = &b_knight; break;
                case PieceType::Bishop: tex = &b_bishop; break;
                case PieceType::Rook:   tex = &b_rook;   break;
                case PieceType::Queen:  tex = &b_queen;  break;
                default: break;
            }
        } else {
            switch (type) {
                case PieceType::Pawn:   tex = &w_pawn;   break;
                case PieceType::Knight: tex = &w_knight; break;
                case PieceType::Bishop: tex = &w_bishop; break;
                case PieceType::Rook:   tex = &w_rook;   break;
                case PieceType::Queen:  tex = &w_queen;  break;
                default: break;
            }
        }

        if (tex) {
            DrawTextureEx(*tex, { currentX, currentY }, 0.0f, CAPTURED_PIECE_SCALE, WHITE);
            currentX += capturedSpacing;
            if (currentX > WIDTH + PANEL_WIDTH - panelSidePadding - capturedSpacing) {
                currentX = capturedX;
                currentY += capturedSpacing + 4.0f;
            }
        }
    }

    float buttonWidth = PANEL_WIDTH - panelSidePadding * 2.0f;
    float buttonX = WIDTH + (PANEL_WIDTH - buttonWidth) * 0.5f;
    float firstButtonY = playerTextY + playerTextSize.y + buttonsTopSpacing;

    resignButton = { buttonX, firstButtonY, buttonWidth, buttonHeight };
    offerDrawButton = { buttonX, firstButtonY + buttonHeight + buttonSpacing, buttonWidth, buttonHeight };
    showResultsButton = { buttonX, firstButtonY + (buttonHeight + buttonSpacing) * 2.0f, buttonWidth, buttonHeight };
    if (pvpMode) {
        undoButton = { 0.0f, 0.0f, 0.0f, 0.0f };
    } else {
        undoButton = { buttonX, firstButtonY + (buttonHeight + buttonSpacing) * 3.0f, buttonWidth, buttonHeight };
    }

    renderButton(resignButton, "Resign", input.isResignClicked(), false, Color{ 238, 238, 210, 255 });
    renderButton(offerDrawButton, "Offer Draw", input.isOfferDrawClicked(), false, Color{ 238, 238, 210, 255 });
    renderButton(showResultsButton, "Show Moves", input.isShowResultsClicked(), false, Color{ 238, 238, 210, 255 });
    if (!pvpMode) {
        renderButton(undoButton, "Undo Move", input.isUndoClicked(), false, LIGHTGRAY);
    }

    const float playerClockBottomPadding = 18.0f;
    const float playerClockHeight = 64.0f;
    Rectangle playerClockRect = {
        buttonX,
        LENGTH - playerClockBottomPadding - playerClockHeight,
        buttonWidth,
        playerClockHeight
    };

    // Captured pieces by bottom player
    PieceColor player1Color = playerPlaysWhite ? PieceColor::White : PieceColor::Black;
    const std::vector<PieceType>& p1CapturedList = (player1Color == PieceColor::White) ? whiteCaptured : blackCaptured;
    PieceColor p1CapturedColor = (player1Color == PieceColor::White) ? PieceColor::Black : PieceColor::White;

    float p1CapturedX = WIDTH + panelSidePadding;
    float p1CapturedY = playerClockRect.y - 40.0f;
    float p1CurrentX = p1CapturedX;
    float p1CurrentY = p1CapturedY;

    for (PieceType type : p1CapturedList) {
        Texture2D* tex = nullptr;
        if (p1CapturedColor == PieceColor::Black) {
            switch (type) {
                case PieceType::Pawn:   tex = &b_pawn;   break;
                case PieceType::Knight: tex = &b_knight; break;
                case PieceType::Bishop: tex = &b_bishop; break;
                case PieceType::Rook:   tex = &b_rook;   break;
                case PieceType::Queen:  tex = &b_queen;  break;
                default: break;
            }
        } else {
            switch (type) {
                case PieceType::Pawn:   tex = &w_pawn;   break;
                case PieceType::Knight: tex = &w_knight; break;
                case PieceType::Bishop: tex = &w_bishop; break;
                case PieceType::Rook:   tex = &w_rook;   break;
                case PieceType::Queen:  tex = &w_queen;  break;
                default: break;
            }
        }

        if (tex) {
            DrawTextureEx(*tex, { p1CurrentX, p1CurrentY }, 0.0f, CAPTURED_PIECE_SCALE, WHITE);
            p1CurrentX += capturedSpacing;
            if (p1CurrentX > WIDTH + PANEL_WIDTH - panelSidePadding - capturedSpacing) {
                p1CurrentX = p1CapturedX;
                p1CurrentY -= (capturedSpacing + 4.0f);
            }
        }
    }

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
