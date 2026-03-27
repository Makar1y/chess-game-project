#include <raylib.h>
#include <string>

// reali versija turėtų turėti bent grid
void DrawPawn(Texture2D pawn, std::string pos) {
    int cell = 80; // 640 po 8
    float scale = ((float)cell / pawn.height) * 0.9f;


    float x = (pos[0] - 'A') * cell + (cell - pawn.width * scale) / 2.0f;
    float y = (8 - (pos[1] - '0')) * cell + (cell - pawn.height * scale) / 2.0f;
    DrawTextureEx(pawn, { x, y }, 0.0f, scale, WHITE);
}

int main() {
    InitWindow(640, 640, "chess");
    Texture2D board = LoadTexture("Textures/board.png");
    Texture2D pawn = LoadTexture("Textures/w_pawn.png");

    SetTextureFilter(board, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(pawn, TEXTURE_FILTER_BILINEAR);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);



        DrawTextureEx(board, {0, 0}, 0.0f, 640.0f / board.width, WHITE);
        DrawPawn(pawn, "E5"); // <----------- Galima padėti pėstininką ant XX



        EndDrawing();
    }

    UnloadTexture(pawn);
    UnloadTexture(board);
    CloseWindow();
}
