#include "Player.hpp"

Player::Player(std::string name, PlayerColor color) {
    this->name = name;
    this->color = color;
}

std::string Player::getName() {
    return name;
}

PlayerColor Player::getColor() {
    return color;
}
