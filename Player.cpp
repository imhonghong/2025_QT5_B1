#include "Player.h"
#include "IGameController.h"
#include "SpriteSheetManager.h"

#include <QTimer>
#include <QDebug>

Player::Player() {
    maxHp = 3;
    currentWaterBombs = 0;
    maxWaterBombs = 1;
}

void Player::setController(IGameController* ctrl) {
    controller = ctrl;
}



void Player::update(float delta) {
    // 未實作，未來可用方向控制移動
}

void Player::enterTrappedState() {
    if (state == PlayerState::Trapped || state == PlayerState::Dead) return;
    state = PlayerState::Trapped;
    if (!trappedTimer) {
        trappedTimer = new QTimer(this);
        trappedTimer->setSingleShot(true);
        connect(trappedTimer, &QTimer::timeout, this, &Player::onTrappedTimeout);
    }
    trappedTimer->start(3000);
    qDebug() << "[Player] 被水球困住，開始漂浮";
}

void Player::tryRescue() {
    if (state != PlayerState::Trapped) return;
    trappedTimer->stop();
    state = PlayerState::Standing; // 或依據當時是否騎 Turtle 調整
    qDebug() << "[Player] 被救出！不扣血";
}

void Player::onTrappedTimeout() {
    if (state != PlayerState::Trapped) return;
    state = PlayerState::Standing;  // 或 TurtleStanding
    takeDamage(1);
    qDebug() << "[Player] 漂浮時間結束，扣血";
}

void Player::addItem(ItemType item) {
    itemSet.insert(item);
}

bool Player::hasItem(ItemType item) const {
    return itemSet.contains(item);
}

void Player::takeDamage(int dmg) {
    hp -= dmg;
    qDebug() << "[Player] 受到傷害，剩餘 HP:" << hp;
    if (hp <= 0) {
        onDie();
    }
}

void Player::onDie() {
    qDebug() << "[Player] 死亡！可加入動畫或結束遊戲邏輯";
    state = PlayerState::Dead;

    if (controller && controller->getMode() == GameMode::Mode1) {
        emit requestEndGame(true);  // Robot 贏
    }
}

QString Player::getFrameKey() const {
    QString dirStr;
    switch (getDirection()) {
        case Direction::Down: dirStr = "down"; break;
        case Direction::Left: dirStr = "left"; break;
        case Direction::Right: dirStr = "right"; break;
        case Direction::Up: dirStr = "up"; break;
    }

    switch (state) {
    case PlayerState::Standing:
        return QString("P_stand_%1_1").arg(dirStr);
    case PlayerState::Walking:
        return QString("P_walk_%1_%2").arg(dirStr).arg(frameIndex % 4);
    case PlayerState::TurtleStanding:
        return QString("PT_walk_%1_1").arg(dirStr);
    case PlayerState::TurtleWalking:
        return QString("PT_walk_%1_%2").arg(dirStr).arg(frameIndex % 2);
    case PlayerState::Trapped:
        return QString("P_wb_%1").arg(frameIndex % 4);
    case PlayerState::Dead:
        return "P_die";
    }
    return "P_stand_down_1";
}

QRect Player::getCollisionBox() const {
    QPixmap sprite = SpriteSheetManager::instance().getFrame(getFrameKey());

    // 假設顯示時是寬度 50 等比縮放
    int displayWidth = 50;
    int displayHeight = sprite.height() * displayWidth / sprite.width();

    QPointF pos = getScreenPos(); // 螢幕座標

    // 回傳圖片下半部作為碰撞區域
    return QRect(
        pos.x(),
        pos.y() + displayHeight - 50, // 從下往上 50
        displayWidth,
        50
    );
}
