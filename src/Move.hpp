#pragma once

class Move {
private:
    int fromX;
    int fromY;
    int toX;
    int toY;

public:
    int getFromX();
    int getFromY();
    int getToX();
    int getToY();

    void setFromX(int x);
    void setFromY(int y);
    void setToX(int x);
    void setToY(int y);
};
