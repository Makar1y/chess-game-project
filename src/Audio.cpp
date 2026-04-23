#include "Audio.hpp"

Audio::Audio()
    : loaded(false) {
    captureSound = Sound{};
    castleSound = Sound{};
    gameEndSound = Sound{};
    gameStartSound = Sound{};
    checkSound = Sound{};
    moveSound = Sound{};
}

void Audio::load() {
    if(loaded) return;

    InitAudioDevice();

    captureSound = LoadSound("Sounds/capture.mp3");
    castleSound = LoadSound("Sounds/castle.mp3");
    gameEndSound = LoadSound("Sounds/game_end.mp3");
    gameStartSound = LoadSound("Sounds/game_start.mp3");
    checkSound = LoadSound("Sounds/check.mp3");
    moveSound = LoadSound("Sounds/move.mp3");

    loaded = true;
}

void Audio::unload() {
    if(!loaded) return;

    UnloadSound(captureSound);
    UnloadSound(castleSound);
    UnloadSound(gameEndSound);
    UnloadSound(gameStartSound);
    UnloadSound(checkSound);
    UnloadSound(moveSound);

    CloseAudioDevice();
    loaded = false;
}

void Audio::playMove() {
    if(loaded) PlaySound(moveSound);
}

void Audio::playCapture() {
    if(loaded) PlaySound(captureSound);
}

void Audio::playCastle() {
    if(loaded) PlaySound(castleSound);
}

void Audio::playCheck() {
    if(loaded) PlaySound(checkSound);
}

void Audio::playGameStart() {
    if(loaded) PlaySound(gameStartSound);
}

void Audio::playGameEnd() {
    if(loaded) PlaySound(gameEndSound);
}

void Audio::playMoveEvent(bool isCapture, bool isCastle, bool isCheck) {
    if(isCheck) {
        playCheck();
        return;
    }

    if(isCastle) {
        playCastle();
        return;
    }

    if(isCapture) {
        playCapture();
        return;
    }

    playMove();
}
