#include "Player.h"

Player::Player() {}

void Player::setController(GameController* ctrl) {
    controller = ctrl;
}

void Player::setPosition(float x, float y) {
    position = QPointF(x, y);
}

void Player::setDirection(int dir) {
    direction = dir;
}

QPointF Player::getPosition() const {
    return position;
}

int Player::getDirection() const {
    return direction;
}

void Player::update(float delta) {
    // 未實作，未來可用方向控制移動
}
