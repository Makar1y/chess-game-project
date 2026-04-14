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
