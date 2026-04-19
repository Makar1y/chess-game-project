#include "../Config.hpp"
#include "Draw.hpp"
#include "Game.hpp"

#include <cmath>

Draw::Draw()
    : input(resignButton, offerDrawButton, showResultsButton, overlayButton, selectWhiteButton, selectBlackButton, startGameButton, selectEloButton) {
    resourcesLoaded = false;

    const float overlayMarginX            = 110.0f;
    const float overlayMarginY            = 80.0f;
    const float overlayButtonWidth        = 240.0f;
    const float buttonOffsetBottom        = 80.0f;
    const float overlayButtonHeight       = 50.0f;

    resignButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    offerDrawButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    showResultsButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };

    // main menu buttons
    startGameButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectWhiteButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectBlackButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
    selectEloButton = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };


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

    EndDrawing();
}

void Draw::render(Board& board, bool pieceSelected, int selectedX, int selectedY, std::set<std::pair<int, int>>& possibleMoves, std::set<std::pair<int, int>>& possibleCaptures, Move* lastMove, bool hasLastMove, OverlayType overlayType, std::string playerName, bool playerPlaysWhite) {
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
