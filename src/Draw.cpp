#include "Draw.hpp"

#define WIDTH 800
#define LENGTH 800
#define PANEL_WIDTH 220

Draw::Draw() {
    resourcesLoaded = false;

    const float sideMargin                = 30.0f;
    const float buttonHeight              = 50.0f;
    const float buttonWidth               = 160.0f;
    const float firstButtonY              = 360.0f;
    const float buttonSpacing             = 70.0f;

    const float overlayMarginX            = 110.0f;
    const float overlayMarginY            = 80.0f;
    const float overlayButtonWidth        = 240.0f;
    const float buttonOffsetBottom        = 80.0f;

    resignButton = { (float)WIDTH + sideMargin, firstButtonY, buttonWidth, buttonHeight };
    offerDrawButton = { (float)WIDTH + sideMargin, firstButtonY + buttonSpacing, buttonWidth, buttonHeight };
    showResultsButton = { (float)WIDTH + sideMargin, firstButtonY + buttonSpacing * 2.0f, buttonWidth, buttonHeight };

    overlayRect = {
        overlayMarginX,
        overlayMarginY,
        (float)(WIDTH + PANEL_WIDTH) - overlayMarginX * 2.0f,
        (float)LENGTH - overlayMarginY * 2.0f
    };

    overlayButton = {
        overlayRect.x + (overlayRect.width - overlayButtonWidth) * 0.5f,
        overlayRect.y + overlayRect.height - buttonOffsetBottom,
        overlayButtonWidth,
        buttonHeight
    };
}

void Draw::loadResources() {
    if (resourcesLoaded) return;

    uiFont = LoadFontEx("Fonts/Rubik-SemiBold.ttf", 96, 0, 0);
    SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);

    boardTexture = LoadTexture("Textures/board.png");
    stockfishTexture = LoadTexture("Textures/stockfish.png");

    b_bishop = LoadTexture("Textures/b_bishop.png");
    b_king   = LoadTexture("Textures/b_king.png");
    b_knight = LoadTexture("Textures/b_knight.png");
    b_pawn   = LoadTexture("Textures/b_pawn.png");
    b_queen  = LoadTexture("Textures/b_queen.png");
    b_rook   = LoadTexture("Textures/b_rook.png");

    w_bishop = LoadTexture("Textures/w_bishop.png");
    w_king   = LoadTexture("Textures/w_king.png");
    w_knight = LoadTexture("Textures/w_knight.png");
    w_pawn   = LoadTexture("Textures/w_pawn.png");
    w_queen  = LoadTexture("Textures/w_queen.png");
    w_rook   = LoadTexture("Textures/w_rook.png");

    SetTextureFilter(stockfishTexture, TEXTURE_FILTER_BILINEAR);

    SetTextureFilter(b_bishop, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(b_king,   TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(b_knight, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(b_pawn,   TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(b_queen,  TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(b_rook,   TEXTURE_FILTER_BILINEAR);

    SetTextureFilter(w_bishop, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(w_king,   TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(w_knight, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(w_pawn,   TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(w_queen,  TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(w_rook,   TEXTURE_FILTER_BILINEAR);

    resourcesLoaded = true;
}

void Draw::unloadResources() {
    if (!resourcesLoaded) return;

    UnloadFont(uiFont);

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
    InitWindow(WIDTH + PANEL_WIDTH, LENGTH, "Chess");
    loadResources();
}

bool Draw::shouldClose() {
    return WindowShouldClose();
}

void Draw::closeWindow() {
    unloadResources();
    CloseWindow();
}

bool Draw::isLeftMousePressed() {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool Draw::isResignClicked() {
    return CheckCollisionPointRec(GetMousePosition(), resignButton);
}

bool Draw::isOfferDrawClicked() {
    return CheckCollisionPointRec(GetMousePosition(), offerDrawButton);
}

bool Draw::isShowResultsClicked() {
    return CheckCollisionPointRec(GetMousePosition(), showResultsButton);
}

bool Draw::isOverlayButtonClicked() {
    return CheckCollisionPointRec(GetMousePosition(), overlayButton);
}

bool Draw::getClickedBoardCell(int& x, int& y) {
    Vector2 mousePos = GetMousePosition();

    x = (int)(mousePos.x / (WIDTH / 8.0f));
    y = (int)(mousePos.y / (LENGTH / 8.0f));

    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

void Draw::render(Board& board, bool pieceSelected, int selectedX, int selectedY, OverlayType overlayType, std::string playerName) {
    static float PIECE_SCALE = 0.6f;
    static float PLAYER_SCALE = 0.2f;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    float boardScale = (float)WIDTH / (float)boardTexture.width;
    DrawTextureEx(boardTexture, { 0.0f, 0.0f }, 0.0f, boardScale, WHITE);

    float cellSize = WIDTH / 8.0f;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece* piece = board.getPiece(x, y);
            if (piece == nullptr) continue;

            if (pieceSelected && x == selectedX && y == selectedY) {
                float centerX = x * cellSize + cellSize * 0.5f;
                float centerY = y * cellSize + cellSize * 0.5f;

                DrawCircle(
                    (int)centerX,
                    (int)centerY,
                    cellSize * 0.4f,
                    Fade(DARKGRAY, 0.8f)
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

            float drawX = x * cellSize + (cellSize - drawW) * 0.5f;
            float drawY = y * cellSize + (cellSize - drawH) * 0.5f;

            DrawTextureEx(*tex, { drawX, drawY }, 0.0f, PIECE_SCALE, WHITE);
        }
    }

    const float playerImageTop          = 60.0f;
    const float playerNameSize          = 26.0f;
    const float playerNameSpacingY      = 15.0f;
    const float buttonSize              = 24.0f;
    const float overlayDim              = 0.25f;
    const float buttonRoundness         = 0.2f;
    const int   buttonSegments          = 8;

    DrawRectangle(WIDTH, 0, PANEL_WIDTH, LENGTH, LIGHTGRAY);
    DrawLine(WIDTH, 0, WIDTH, LENGTH, GRAY);

    float playerImageW = (float)stockfishTexture.width * PLAYER_SCALE;
    float playerImageH = (float)stockfishTexture.height * PLAYER_SCALE;
    float playerImageX = WIDTH + (PANEL_WIDTH - playerImageW) * 0.5f;
    float playerImageY = playerImageTop;

    DrawTextureEx(stockfishTexture, { playerImageX, playerImageY }, 0.0f, PLAYER_SCALE, WHITE);

    Vector2 playerTextSize = MeasureTextEx(uiFont, playerName.c_str(), playerNameSize, 1);
    DrawTextEx(uiFont, playerName.c_str(),
    {
        WIDTH + (PANEL_WIDTH - playerTextSize.x) * 0.5f,
        playerImageY + playerImageH + playerNameSpacingY
    },
    playerNameSize, 1, BLACK);

    Color resignColor = isResignClicked() ? GRAY : LIGHTGRAY;
    Color drawColor = isOfferDrawClicked() ? GRAY : LIGHTGRAY;
    Color resultsColor = isShowResultsClicked() ? GRAY : LIGHTGRAY;

    DrawRectangleRounded(resignButton, buttonRoundness, buttonSegments, resignColor);
    DrawRectangleRounded(offerDrawButton, buttonRoundness, buttonSegments, drawColor);
    DrawRectangleRounded(showResultsButton, buttonRoundness, buttonSegments, resultsColor);

    const char* resignText = "Resign";
    const char* drawTextStr = "Offer Draw";
    const char* resultsText = "Show Results";

    Vector2 resignSize = MeasureTextEx(uiFont, resignText, buttonSize, 1);
    Vector2 drawSize = MeasureTextEx(uiFont, drawTextStr, buttonSize, 1);
    Vector2 resultsSize = MeasureTextEx(uiFont, resultsText, buttonSize, 1);

    DrawTextEx(uiFont, resignText,
        {
            resignButton.x + (resignButton.width - resignSize.x) * 0.5f,
            resignButton.y + (resignButton.height - resignSize.y) * 0.5f
        },
        buttonSize, 1, BLACK);

    DrawTextEx(uiFont, drawTextStr,
        {
            offerDrawButton.x + (offerDrawButton.width - drawSize.x) * 0.5f,
            offerDrawButton.y + (offerDrawButton.height - drawSize.y) * 0.5f
        },
        buttonSize, 1, BLACK);

    DrawTextEx(uiFont, resultsText,
        {
            showResultsButton.x + (showResultsButton.width - resultsSize.x) * 0.5f,
            showResultsButton.y + (showResultsButton.height - resultsSize.y) * 0.5f
        },
        buttonSize, 1, BLACK);

    if (overlayType != OverlayType::None) {
        DrawRectangle(0, 0, WIDTH + PANEL_WIDTH, LENGTH, Fade(BLACK, overlayDim));
        DrawRectangleRec(overlayRect, RAYWHITE);
        DrawRectangleLinesEx(overlayRect, 2, GRAY);

        const char* overlayButtonText = (overlayType == OverlayType::Results) ? "Back to the game" : "New game";
        Vector2 overlayButtonTextSize = MeasureTextEx(uiFont, overlayButtonText, buttonSize, 1);

        Color overlayButtonColor = isOverlayButtonClicked() ? GRAY : LIGHTGRAY;

        DrawRectangleRounded(overlayButton, buttonRoundness, buttonSegments, overlayButtonColor);
        DrawTextEx(uiFont, overlayButtonText,
            {
                overlayButton.x + (overlayButton.width - overlayButtonTextSize.x) * 0.5f,
                overlayButton.y + (overlayButton.height - overlayButtonTextSize.y) * 0.5f
            },
            buttonSize, 1, BLACK);
    }

    EndDrawing();
}
