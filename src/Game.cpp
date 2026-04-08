#include "Game.hpp"

#include <cmath>

#define WIDTH 600
#define LENGTH 600

bool Game::validateMove(Move move) {
    //TODO
    return true;
}

void Game::updateBoard(Move move) {

    static bool pieceSelected = false;
    static int selectedX = -1;
    static int selectedY = -1;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        int x = (int)(mousePos.x / (WIDTH / 8.0f));
        int y = (int)(mousePos.y / (LENGTH / 8.0f));

        if (x >= 0 && x < 8 && y >= 0 && y < 8) {
            Piece* clickedPiece = board.getPiece(x, y);

            if (!pieceSelected) {
                if (clickedPiece != nullptr) {
                    pieceSelected = true;
                    selectedX = x;
                    selectedY = y;
                }
            } else {
                if (x == selectedX && y == selectedY) {
                    pieceSelected = false;
                    selectedX = -1;
                    selectedY = -1;
                } else {
                    Move newMove;
                    newMove.setFromX(selectedX);
                    newMove.setFromY(selectedY);
                    newMove.setToX(x);
                    newMove.setToY(y);

                    if (validateMove(newMove)) {
                        board.update(newMove);
                    }

                    pieceSelected = false;
                    selectedX = -1;
                    selectedY = -1;
                }
            }
        }
    }

    static Texture2D boardTexture = LoadTexture("Textures/board.png");

    static Texture2D b_bishop = LoadTexture("Textures/b_bishop.png");
    static Texture2D b_king   = LoadTexture("Textures/b_king.png");
    static Texture2D b_knight = LoadTexture("Textures/b_knight.png");
    static Texture2D b_pawn   = LoadTexture("Textures/b_pawn.png");
    static Texture2D b_queen  = LoadTexture("Textures/b_queen.png");
    static Texture2D b_rook   = LoadTexture("Textures/b_rook.png");

    static Texture2D w_bishop = LoadTexture("Textures/w_bishop.png");
    static Texture2D w_king   = LoadTexture("Textures/w_king.png");
    static Texture2D w_knight = LoadTexture("Textures/w_knight.png");
    static Texture2D w_pawn   = LoadTexture("Textures/w_pawn.png");
    static Texture2D w_queen  = LoadTexture("Textures/w_queen.png");
    static Texture2D w_rook   = LoadTexture("Textures/w_rook.png");

    static bool filterSet = false;

    if (!filterSet) {
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

        filterSet = true;
    }

    static float PIECE_SCALE = 0.5f;

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

            float drawX = roundf(x * cellSize + (cellSize - drawW) * 0.5f);
            float drawY = roundf(y * cellSize + (cellSize - drawH) * 0.5f);

            DrawTextureEx(*tex, { drawX, drawY }, 0.0f, PIECE_SCALE, WHITE);
        }
    }

    EndDrawing();
}
void Game::startGame() {
    InitWindow(WIDTH, LENGTH, "Chess");

    while (!WindowShouldClose()) {
        updateBoard(Move());
    }

    CloseWindow();
}

bool Game::makeMove(Move move) {
    //TODO
    return true;
}

void Game::resign(Player player) {
    //TODO
}

void Game::offerDraw(Player player) {
    //TODO
}

void Game::showResults() {
    //TODO
}
