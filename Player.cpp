#include "Player.h"
#include "IGameController.h"

Player::Player() {}

void Player::setController(IGameController* ctrl) {
    controller = ctrl;
}



void Player::update(float delta) {
    // 未實作，未來可用方向控制移動
}
