#include "Player.h"
#include "IGameController.h"
#include "SpriteSheetManager.h"
#include "BattleScene.h"

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
    if (state == PlayerState::Dead || state == PlayerState::Trapped) return;
    if (activeKeys.isEmpty()) {
        state = PlayerState::Standing;
        return;
    }

    // 取得其中一個按下的方向（可以加權或優先順序）
    Direction dir = *activeKeys.begin();  // 先取第一個方向
    currentMoveDir = dir;
    setDirection(dir); // for frame 顯示

    float step = moveSpeed * delta;
    QPointF offset;

    switch (dir) {
        case Direction::Up:    offset = QPointF(0, -step); break;
        case Direction::Down:  offset = QPointF(0, +step); break;
        case Direction::Left:  offset = QPointF(-step, 0); break;
        case Direction::Right: offset = QPointF(+step, 0); break;
        default: return;
    }

    // 檢查是否碰撞（以 offset 為基礎）
    QRect newBox = getCollisionBox().translated(offset.toPoint());
    if (!scene->checkCollision(newBox)) {
        screenPos += offset;
        state = PlayerState::Walking;
    } else {
        state = PlayerState::Standing;
    }

    if (state == PlayerState::Walking) {
        frameIndex++;
    }

    qDebug() << "[Player::update] 移動中: state=" << static_cast<int>(state)/* << " pos=" << screenPos*/;
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
    if (item == ItemType::ExtraBomb)
        extraBombCount++;
    else
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
        return QString("P_walk_%1_%2").arg(dirStr).arg(frameIndex % 4 + 1);
    case PlayerState::TurtleStanding:
        return QString("PT_walk_%1_1").arg(dirStr);
    case PlayerState::TurtleWalking:
        return QString("PT_walk_%1_%2").arg(dirStr).arg(frameIndex % 2 + 1);
    case PlayerState::Trapped:
        return QString("P_wb_%1").arg(frameIndex % 4  +1);
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
        pos.y() + displayHeight - 46, // 從下往上 50
        46,
        46
    );
}

void Player::setScene(BattleScene *s){
    scene = s;
}

void Player::addMoveKey(Direction dir) {
    activeKeys.insert(dir);
    qDebug() << "[Player] 按下方向鍵:" << static_cast<int>(dir);
}

void Player::removeMoveKey(Direction dir) {
    activeKeys.remove(dir);
}

QPoint Player::getNearestGridPos() const {
    QRect box = getCollisionBox();
    QPoint topLeft = box.topLeft();

    QPoint gridPos(qRound(topLeft.x() / 50.0), qRound(topLeft.y() / 50.0));
    qDebug() << "[Player] 碰撞框左上角:" << topLeft << "對應格子:" << gridPos;

    return gridPos;
}

void Player::tryPlaceWaterBomb() {
    if (!scene) {
        qDebug() << "[Player] scene is nullptr";
        return;
    }

    int maxBombs = 1 + extraBombCount;
    int current = scene->getWaterBombCount(this);

    QPoint gridPos = getNearestGridPos();
    int tile = scene->getMap(gridPos);

    qDebug() << "[Player] 試圖放炸彈: gridPos=" << gridPos
             << "tile=" << tile << " bomb=" << current << "/" << maxBombs;

    if (tile == 1 || tile == 2) {
        qDebug() << "[Player] 格子為牆或磚";
        return;
    }
    if (scene->hasWaterBomb(gridPos)) {
        qDebug() << "[Player] 該格已有水球";
        return;
    }
    if (current >= maxBombs) {
        qDebug() << "[Player] 已達水球上限";
        return;
    }

    scene->addWaterBomb(gridPos, this);
    qDebug() << "[Player] 放置炸彈成功";
}

