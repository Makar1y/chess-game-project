#include "Move.hpp"

int Move::getFromX() {
    return fromX;
}

int Move::getFromY() {
    return fromY;
}

int Move::getToX() {
    return toX;
}

int Move::getToY() {
    return toY;
}

bool Move::getIsFirstMove() {
    return isFirstMove;
}

std::unique_ptr<Piece> Move::getCapturedPiece() {
    return std::move(capturedPiece);
}

void Move::setFromX(int x) {
    fromX = x;
}

void Move::setFromY(int y) {
    fromY = y;
}

void Move::setToX(int x) {
    toX = x;
}

void Move::setToY(int y) {
    toY = y;
}

void Move::setIsFirstMove(bool isFirstMove) {
    this->isFirstMove = isFirstMove;
}

void Move::setCapturedPiece(std::unique_ptr<Piece> piece) {
    capturedPiece = std::move(piece);
}

