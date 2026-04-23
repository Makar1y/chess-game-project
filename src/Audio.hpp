#pragma once

#include "raylib.h"

class Audio {
private:
    Sound captureSound;
    Sound castleSound;
    Sound gameEndSound;
    Sound gameStartSound;
    Sound checkSound;
    Sound moveSound;
    bool loaded;

public:
    Audio();

    void load();
    void unload();

    void playMove();
    void playCapture();
    void playCastle();
    void playCheck();
    void playGameStart();
    void playGameEnd();

    void playMoveEvent(bool isCapture, bool isCastle, bool isCheck);
};
