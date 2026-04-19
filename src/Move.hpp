#pragma once

#include "Piece.hpp"
#include <memory>

class Move {
private:
    int fromX;
    int fromY;
    int toX;
    int toY;
    bool isFirstMove;

    std::unique_ptr<Piece> capturedPiece;

public:
    int getFromX();
    int getFromY();
    int getToX();
    int getToY();
    bool getIsFirstMove();
    std::unique_ptr<Piece> getCapturedPiece();

    void setFromX(int x);
    void setFromY(int y);
    void setToX(int x);
    void setToY(int y);
    void setIsFirstMove(bool isFirstMove);
    void setCapturedPiece(std::unique_ptr<Piece> piece);
};
