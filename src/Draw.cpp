#include "../Config.hpp"
#include "Draw.hpp"
#include "Game.hpp"

Draw::Draw()
    : input(resignButton, offerDrawButton, showResultsButton, overlayButton) {
    resourcesLoaded = false;

    const float overlayMarginX            = 110.0f;
    const float overlayMarginY            = 80.0f;
    const float overlayButtonWidth        = 240.0f;
    const float buttonOffsetBottom        = 80.0f;
    const float overlayButtonHeight       = 50.0f;

    resignButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    offerDrawButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    showResultsButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };

    overlayRect = Rectangle{
        overlayMarginX,
        overlayMarginY,
        (float)(WIDTH + PANEL_WIDTH) - overlayMarginX * 2.0f,
        (float)LENGTH - overlayMarginY * 2.0f
    };

    overlayButton = Rectangle{
        overlayRect.x + (overlayRect.width - overlayButtonWidth) * 0.5f,
        overlayRect.y + overlayRect.height - buttonOffsetBottom,
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
    SetTargetFPS(60);
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

void Draw::mainMenu() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    const float titleSize = 36.0f;
    const float subtitleSize = 22.0f;
    const float buttonWidth = 320.0f;
    const float buttonHeight = 58.0f;
    const float buttonSpacing = 24.0f;
    const float buttonRoundness = 0.22f;
    const int buttonSegments = 10;
    const float titleTop = 120.0f;
    const float buttonsTop = 280.0f;

    const char* titleText = "Chess";
    const char* subtitleText = "Main Menu";
    const char* startText = "Start Game";
    const char* colorText = PLAYER_PLAYS_WHITE ? "Color: White" : "Color: Black";


    const char* eloText = "Stockfish Elo: 1320";

    Vector2 titleSizePx = MeasureTextEx(uiFont36, titleText, titleSize, 1);
    Vector2 subtitleSizePx = MeasureTextEx(uiFont22, subtitleText, subtitleSize, 1);

    const float centerX = (WIDTH + PANEL_WIDTH) * 0.5f;
    const float buttonX = centerX - buttonWidth * 0.5f;

    Rectangle startButton = { buttonX, buttonsTop, buttonWidth, buttonHeight };
    Rectangle colorButton = { buttonX, buttonsTop + buttonHeight + buttonSpacing, buttonWidth, buttonHeight };
    Rectangle eloButton = { buttonX, buttonsTop + (buttonHeight + buttonSpacing) * 2.0f, buttonWidth, buttonHeight };

    Vector2 mousePos = GetMousePosition();
    Color defaultButtonColor = Color{ 238, 238, 210, 255 };
    Color hoveredButtonColor = Color{ 218, 218, 190, 255 };
    Color accentColor = Color{ 117, 148, 85, 255 };

    DrawRectangleGradientV(0, 0, WIDTH + PANEL_WIDTH, LENGTH, Color{ 245, 245, 240, 255 }, Color{ 224, 232, 214, 255 });
    DrawRectangleRounded(Rectangle{ centerX - 240.0f, 70.0f, 480.0f, 640.0f }, 0.08f, 12, Fade(WHITE, 0.8f));
    DrawRectangleLinesEx(Rectangle{ centerX - 240.0f, 70.0f, 480.0f, 640.0f }, 2.0f, Fade(accentColor, 0.4f));

    DrawTextEx(uiFont36, titleText,
        { roundf(centerX - titleSizePx.x * 0.5f), titleTop },
        titleSize, 1, accentColor);
    DrawTextEx(uiFont22, subtitleText,
        { roundf(centerX - subtitleSizePx.x * 0.5f), titleTop + titleSizePx.y + 12.0f },
        subtitleSize, 1, DARKGRAY);

    Color startButtonColor = CheckCollisionPointRec(mousePos, startButton) ? hoveredButtonColor : defaultButtonColor;
    Color colorButtonColor = CheckCollisionPointRec(mousePos, colorButton) ? hoveredButtonColor : defaultButtonColor;
    Color eloButtonColor = CheckCollisionPointRec(mousePos, eloButton) ? hoveredButtonColor : defaultButtonColor;

    DrawRectangleRounded(startButton, buttonRoundness, buttonSegments, startButtonColor);
    DrawRectangleRounded(colorButton, buttonRoundness, buttonSegments, colorButtonColor);
    DrawRectangleRounded(eloButton, buttonRoundness, buttonSegments, eloButtonColor);

    Vector2 startSize = MeasureTextEx(uiFont22, startText, subtitleSize, 1);
    Vector2 colorSize = MeasureTextEx(uiFont22, colorText, subtitleSize, 1);
    Vector2 eloSize = MeasureTextEx(uiFont22, eloText, subtitleSize, 1);

    DrawTextEx(uiFont22, startText,
        { roundf(startButton.x + (startButton.width - startSize.x) * 0.5f), roundf(startButton.y + (startButton.height - startSize.y) * 0.5f) },
        subtitleSize, 1, BLACK);
    DrawTextEx(uiFont22, colorText,
        { roundf(colorButton.x + (colorButton.width - colorSize.x) * 0.5f), roundf(colorButton.y + (colorButton.height - colorSize.y) * 0.5f) },
        subtitleSize, 1, BLACK);
    DrawTextEx(uiFont22, eloText,
        { roundf(eloButton.x + (eloButton.width - eloSize.x) * 0.5f), roundf(eloButton.y + (eloButton.height - eloSize.y) * 0.5f) },
        subtitleSize, 1, BLACK);

    EndDrawing();
}

void Draw::render(Board& board, bool pieceSelected, int selectedX, int selectedY, OverlayType overlayType, std::string playerName, bool playerPlaysWhite) {
    static float PIECE_SCALE = 0.46f;
    static float PLAYER_SCALE = 0.28f;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    float boardScale = (float)WIDTH / (float)boardTexture.width;
    DrawTextureEx(boardTexture, { 0.0f, 0.0f }, 0.0f, boardScale, WHITE);

    float cellSize = WIDTH / 8.0f;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece* piece = board.getPiece(x, y);
            if (piece == nullptr) continue;

            int drawBoardX = playerPlaysWhite ? x : 7 - x;
            int drawBoardY = playerPlaysWhite ? y : 7 - y;

            if (pieceSelected && x == selectedX && y == selectedY) {
                float centerX = drawBoardX * cellSize + cellSize * 0.5f;
                float centerY = drawBoardY * cellSize + cellSize * 0.5f;

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

Color resignColor = input.isResignClicked()
    ? Color{ 218, 218, 190, 255 }
    : Color{ 238, 238, 210, 255 };

Color drawColor = input.isOfferDrawClicked()
    ? Color{ 218, 218, 190, 255 }
    : Color{ 238, 238, 210, 255 };

Color resultsColor = input.isShowResultsClicked()
    ? Color{ 218, 218, 190, 255 }
    : Color{ 238, 238, 210, 255 };

    DrawRectangleRounded(resignButton, buttonRoundness, buttonSegments, resignColor);
    DrawRectangleRounded(offerDrawButton, buttonRoundness, buttonSegments, drawColor);
    DrawRectangleRounded(showResultsButton, buttonRoundness, buttonSegments, resultsColor);

    const char* resignText = "Resign";
    const char* drawTextStr = "Offer Draw";
    const char* resultsText = "Show Results";

    Vector2 resignSize = MeasureTextEx(uiFont22, resignText, 22, 1);
    Vector2 drawSize = MeasureTextEx(uiFont22, drawTextStr, 22, 1);
    Vector2 resultsSize = MeasureTextEx(uiFont22, resultsText, 22, 1);

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

    if (overlayType != OverlayType::None) {
        DrawRectangle(0, 0, WIDTH + PANEL_WIDTH, LENGTH, Fade(BLACK, 0.25f));
        DrawRectangleRec(overlayRect, RAYWHITE);
        DrawRectangleLinesEx(overlayRect, 2, GRAY);

        const char* overlayButtonText = (overlayType == OverlayType::Results) ? "Back to the game" : "New game";
        Vector2 overlayButtonTextSize = MeasureTextEx(uiFont22, overlayButtonText, 22, 1);

        Color overlayButtonColor = input.isOverlayButtonClicked() ? GRAY : LIGHTGRAY;

        DrawRectangleRounded(overlayButton, buttonRoundness, buttonSegments, overlayButtonColor);
        DrawTextEx(uiFont22, overlayButtonText,
            {
                roundf(overlayButton.x + (overlayButton.width - overlayButtonTextSize.x) * 0.5f),
                roundf(overlayButton.y + (overlayButton.height - overlayButtonTextSize.y) * 0.5f)
            },
            22, 1, BLACK);
    }

    EndDrawing();
}
