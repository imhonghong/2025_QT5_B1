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

    if (state == PlayerState::Dead) {
        qDebug() << "[Player] is dead, frame" << frameIndex;
        frameIndex++; // 可視為播放動畫時的更新
        return;
    }

    if (state == PlayerState::Trapped) {
        // 不要更新 frameIndex，改由 trappedAnimTimer 控制動畫播放
        return;
    }

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

    // 玩家移動後的 collisionBox
    QRect box = getCollisionBox();

    for (WaterBomb* bomb : scene->getWaterBombs()) {
        if (bomb->getOwner() == this) {
            QRect bombBox(bomb->getGridPos().x() * 50, bomb->getGridPos().y() * 50, 50, 50);
            if (!box.intersects(bombBox)) {
                bomb->setPlayerHasLeft(true);  // ✅ 呼叫 set flag
            }
        }
    }

    if (invincible) invincibleFrameCounter++;

    qDebug() << "[Player::update] 移動中: state=" << static_cast<int>(state)/* << " pos=" << screenPos*/;
}

void Player::setStateStanding() {
    if (hasItem(ItemType::Turtle))
        state = PlayerState::TurtleStanding;
    else
        state = PlayerState::Standing;
}

void Player::enterTrappedState() {
    if (state == PlayerState::Trapped) return;

    qDebug() << "[Player] Entering trapped state";
    state = PlayerState::Trapped;
    trappedFrameIndex = 0;

    if (trappedAnimTimer) {
        trappedAnimTimer->stop();
        delete trappedAnimTimer;
    }
    trappedAnimTimer = new QTimer(this);
    connect(trappedAnimTimer, &QTimer::timeout, this, [=]() {
        if (++trappedFrameIndex >= 4) { // 只播4張圖
            trappedAnimTimer->stop();
        }
    });
    trappedAnimTimer->start(750); // 播 4 張圖 / 3 秒 = 每張 750ms

    if (trappedTimer) {
        trappedTimer->stop();
        delete trappedTimer;
    }
    trappedTimer = new QTimer(this);
    connect(trappedTimer, &QTimer::timeout, this, &Player::onTrappedTimeout);
    trappedTimer->setSingleShot(true);
    trappedTimer->start(3000); // 3 秒後觸發
}


void Player::tryRescue() {
    if (state != PlayerState::Trapped) return;
    trappedTimer->stop();
    state = PlayerState::Standing; // 或依據當時是否騎 Turtle 調整
    qDebug() << "[Player] 被救出！不扣血";
}

void Player::onTrappedTimeout() {
    qDebug() << "[Player] Trapped time up!";

    if (hasItem(ItemType::Needle)) {
        needleCount--;
        removeItem(ItemType::Needle);
        setStateStanding();
    } else {
        // 播 P_wd_1 / 2 共 0.5 秒再 respawn
        state = PlayerState::Dead;
        recoverFrameCount = 0;

        if (recoverTimer) {
            recoverTimer->stop();
            delete recoverTimer;
        }
        recoverTimer = new QTimer(this);
        connect(recoverTimer, &QTimer::timeout, this, [=]() {
            if (++recoverFrameCount >= 2) {
                recoverTimer->stop();
                takeDamage(1);
                setGridPos(getStartPos(0));
                qDebug() <<"[Player]onTrappedTimeout() startGridPos" <<startGridPos;
                setStateStanding();
            }
        });
        recoverTimer->start(250); // 兩張共0.5秒
    }
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
    if (invincible || isDead()) {
        qDebug() << "[Player] 無敵中，忽略傷害";
        return;
    }

    hp -= dmg;
    qDebug() << "[Player] 受到傷害，剩餘 HP:" << hp;

    if (hp <= 0) {
        onDie();
        return;
    }

    invincible = true;
    qDebug() << "[Player] 進入無敵狀態";

    if (!invincibleTimer) {
        invincibleTimer = new QTimer(this);
        invincibleTimer->setSingleShot(true);
        connect(invincibleTimer, &QTimer::timeout, this, [this]() {
            invincible = false;
            qDebug() << "[Player] 無敵結束";
        });
    }
    invincibleTimer->start(2000);
}


void Player::onDie() {
    qDebug() << "[Player] 死亡！可加入動畫或結束遊戲邏輯";

    if (controller && controller->getMode() == GameMode::Mode1) {
        emit requestEndGame(true);  // Robot 贏
    }
    if (controller && controller->getMode() == GameMode::Mode2) {
        emit requestEndGame(false);  // player 輸
    }
}

QString Player::getFrameKey() const {
    QString dirStr;

    if (invincible && (invincibleFrameCounter / 4) % 2 == 0) {
        return "";  // 空字串表示不顯示
    }

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
        return QString("P_walk_%1_%2").arg(dirStr).arg(frameIndex % 6 + 1);
    case PlayerState::TurtleStanding:
        return QString("PT_walk_%1_1").arg(dirStr);
    case PlayerState::TurtleWalking:
        return QString("PT_walk_%1_%2").arg(dirStr).arg(frameIndex % 2 + 1);
    case PlayerState::Trapped:
        return QString("P_wb_%1").arg(trappedFrameIndex  % 4  +1);
    case PlayerState::Dead:
        return QString("P_wd_%1").arg(frameIndex %2 +1);
    }
    return "P_stand_down_1";
}

QRect Player::getCollisionBox() const {
    QString key = getFrameKey();
    if (key.isEmpty()) {
        // 若為無敵閃爍，則保留固定大小碰撞框
        QPointF pos = getScreenPos();
        return QRect(pos.x(), pos.y(), 46, 46);
    }

    QPixmap sprite = SpriteSheetManager::instance().getFrame(key);
    if (sprite.isNull()) {
        QPointF pos = getScreenPos();
        return QRect(pos.x(), pos.y(), 46, 46);
    }

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

QPoint Player::getStartPos(int waveIndex){
    if (waveIndex == 0){
        return QPoint(5,8);
    } else if (waveIndex == 1){
        return QPoint(5,8);
    }
    else{
        return QPoint(0,0);
    }
}
