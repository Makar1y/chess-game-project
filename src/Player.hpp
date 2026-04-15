#pragma once

#include <string>
#include "Move.hpp"

enum class PlayerColor {
    White,
    Black
};

class Player {
private:
    std::string name;
    PlayerColor color;
    bool inCheck = false;

public:
    Player(std::string name, PlayerColor color);

    std::string getName();
    PlayerColor getColor();

    bool getInCheck();
    void setInCheck(bool value);
};
