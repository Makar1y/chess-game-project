#include "Player.hpp"

Player::Player(std::string name, PlayerColor color) {
    this->name = name;
    this->color = color;
    this->inCheck = false;
}

std::string Player::getName() {
    return name;
}

PlayerColor Player::getColor() {
    return color;
}

bool Player::getInCheck() {
    return inCheck;
}

void Player::setInCheck(bool value) {
    inCheck = value;
}

void Player::setColor(PlayerColor color) {
    this->color = color;
}

void Player::setName(const std::string& name) {
    this->name = name;
}
