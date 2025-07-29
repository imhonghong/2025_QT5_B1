#include "Player.h"
#include "IGameController.h"
#include "SpriteSheetManager.h"
#include "BattleScene.h"

#include <QTimer>
#include <QDebug>

Player::Player() {
    maxHp = 3;
    setCurrentWaterBombs(0);
    setMaxWaterBombs(1);
}

void Player::setController(IGameController* ctrl) {
    controller = ctrl;
}

void Player::update(float delta) {

    if (state == PlayerState::Dead) {
        // qDebug() << "[Player] is dead, frame" << frameIndex;
        frameIndex++; // 可視為播放動畫時的更新
        return;
    }

    if (state == PlayerState::Trapped) {
        // 不要更新 frameIndex，改由 trappedAnimTimer 控制動畫播放
        return;
    }

    if (activeKeys.isEmpty()) {
        setStateStanding();
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
    if (!scene->checkCollision(newBox) || scene->tryPushBrick(getCollisionBox(), currentMoveDir)) {
        screenPos += offset;

        // 地圖邊界
        constexpr int mapWidth = 11;
        constexpr int mapHeight = 9;
        int displayHeight = (hasItem(ItemType::Turtle)) ? 79 : 56;
        int minY = -displayHeight + 46;  // ✅ 圖片最上緣可以超出地圖
        int maxY = (mapHeight - 1) * 50;

        if (hasItem(ItemType::MoonWalk)) {
            QRect box = getCollisionBox(); // ✅ 取得目前碰撞框

            if (box.right() <= 0) {
                screenPos.setX((mapWidth - 1) * 50); // 從右邊出現
            } else if (box.left() >= mapWidth * 50) {
                screenPos.setX(0); // 從左邊出現
            }

            if (box.bottom() <= 0) {
                screenPos.setY((mapHeight - 1) * 50 + displayHeight - 46); // 從下邊出現
            } else if (box.top() >= mapHeight * 50) {
                screenPos.setY(-displayHeight + 46); // 從上邊出現
            }
        } else {  // 沒有 MoonWalk 時，限制邊界
            screenPos.setX(qBound(0.0, screenPos.x(), (mapWidth - 1) * 50.0));
            screenPos.setY(qBound(static_cast<double>(minY), screenPos.y(), static_cast<double>(maxY)));
        }

        // ✅ 成功移動後決定狀態
        if (hasItem(ItemType::Turtle))
            state = PlayerState::TurtleWalking;
        else
            state = PlayerState::Walking;

        frameIndex++;
    } else {
        setStateStanding();
    }

    if (state == PlayerState::Walking) {
        frameIndex++;
    }

    if (scene) {
        QRect playerBox = getCollisionBox();
        auto& items = scene->getItems();

        for (int i = items.size() - 1; i >= 0; --i) {
            Item* item = items[i];
            QRect itemBox(item->getScreenPos(), QSize(50, 50));

            if (playerBox.intersects(itemBox)) {
                addItem(item->getType());
                qDebug() << "[Player] 撿到 item:" << item->getName();
                delete item;
                items.removeAt(i);
            }
        }
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


void Player::tryUseNeedle() {
    if (state != PlayerState::Trapped || needleCount <= 0) return;

    qDebug() << "[Player] 使用針戳破水球！";
    trappedTimer->stop();
    if (trappedAnimTimer) {
        trappedAnimTimer->stop();
        delete trappedAnimTimer;
        trappedAnimTimer = nullptr;
        trappedTimer->stop();
        delete trappedTimer;
        trappedTimer = nullptr;
    }

    needleCount--;
    removeItem(ItemType::Needle);
    setStateStanding();

    // 無敵兩秒
    invincible = true;
    invincibleFrameCounter = 0;

    if (!invincibleTimer) {
        invincibleTimer = new QTimer(this);
        invincibleTimer->setSingleShot(true);
        connect(invincibleTimer, &QTimer::timeout, this, [this]() {
            invincible = false;
            qDebug() << "[Player] 無敵結束 (使用針)";
        });
    }
    invincibleTimer->start(2000);
}


void Player::onTrappedTimeout() {
    qDebug() << "[Player] Trapped time up!";

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
            qDebug() <<"[Player]onTrappedTimeout() startGridPos" <<startGridPos;
            setStateStanding();
        }
    });
    recoverTimer->start(250); // 兩張共0.5秒
}


void Player::addItem(ItemType item) {
    switch (item) {
        case ItemType::Needle:
            needleCount++;
            qDebug() << "[Player] 拾取 Needle, 總數=" << needleCount;
            break;
    case ItemType::ExtraBomb: {
            if (maxWaterBombs < 5) {
                ++maxWaterBombs;
                qDebug() << "[Player] 撿到 ExtraBomb，最大水球數變為:" << maxWaterBombs;
            } else {
                qDebug() << "[Player] 已達水球數上限，不再增加";
            }
            break;
        }
        case ItemType::SpeedShoes:{
           itemSet.insert(item);
            updateMoveSpeed();
            qDebug() << "[Player] 拾取 SpeedShoes, 移動速度=" << moveSpeed;
            break;
        }
        case ItemType::Turtle:{
            itemSet.insert(item);
            state = PlayerState::TurtleStanding;
            updateMoveSpeed();
            break;
        }
        case ItemType::Glove:
            if (!hasGlove) {
                hasGlove = true;
                gloveCount = 3;
                itemSet.insert(item);
                qDebug() << "[Player] 拾取 Glove";
            } else {
                qDebug() << "[Player] 已有 Glove，略過刷新";
            }
            break;
        case ItemType::MoonWalk:
            if (moonwalkActive) {
                qDebug() << "[Player] MoonWalk 仍在作用中，忽略拾取";
                return;
            }

            moonwalkActive = true;
            itemSet.insert(item);
            qDebug() << "[Player] 拾取 MoonWalk，啟動計時";

            if (!moonwalkTimer) {
                moonwalkTimer = new QTimer(this);
                moonwalkTimer->setSingleShot(true);
                connect(moonwalkTimer, &QTimer::timeout, this, [this]() {
                    moonwalkActive = false;
                    removeItem(ItemType::MoonWalk);
                    qDebug() << "[Player] MoonWalk 效果結束";
                });
            }
            moonwalkTimer->start(10000); // 10 秒
            break;
        case ItemType::PowerPotion:
            powerPotionCount++;
            itemSet.insert(item);
            qDebug() << "[Player] 拾取 " << static_cast<int>(item) << "(特殊道具)";
            break;

        default:
            qWarning() << "[Player] 未知道具";
            break;
    }
}


bool Player::hasItem(ItemType item) const {
    return itemSet.contains(item);
}

void Player::takeDamage(int dmg) {
    if (invincible || isDead()) {
        qDebug() << "[Player] 無敵中，忽略傷害";
        return;
    }

    if (hasItem(ItemType::Turtle)) {
        qDebug() << "[Player] 被撞到但有烏龜，進入破殼狀態";
        onTurtleBreak();
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
            setGridAlignedScreenPos(getStartPos(currentWaveIndex));  // 使用已存的 waveIndex
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
    int displayHeight = (hasItem(ItemType::Turtle))? 79 : 56 ;
    QPointF pos = getScreenPos(); // 螢幕座標

    // 回傳圖片下半部作為碰撞區域
    int baseX = pos.x();
    int baseY = pos.y() + displayHeight - 46;

    return QRect(baseX+2, baseY, 46, 46);
}

void Player::setScene(BattleScene *s){
    scene = s;
}

void Player::addMoveKey(Direction dir) {
    activeKeys.insert(dir);
    // qDebug() << "[Player] 按下方向鍵:" << static_cast<int>(dir);
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
    if (!scene) return;
    qDebug() << "[Player] 檢查炸彈狀態 current/max:" << currentWaterBombs << "/" << maxWaterBombs;

    QPoint gridPos = getNearestGridPos();
    int tile = scene->getMap(gridPos);

    if (tile == 1 || tile == 2) {
        qDebug() << "[Player] 該格為牆或磚，無法放置";
        return;
    }

    if (scene->hasWaterBomb(gridPos)) {
        qDebug() << "[Player] 該格已有水球";
        return;
    }

    if (!canPlaceBomb()) {
        qDebug() << "[Player] 已達最大水球數量上限";
        return;
    }

    int range = 1 + powerPotionCount;

    scene->addWaterBomb(gridPos, this, range, false);
    qDebug() << "[player:tryPlaceBomb] range=" << range;
    increaseCurrentWaterBombs();  // 放置後增加數量

    qDebug() << "[Player] 放置炸彈成功，當前數量:"
             << getCurrentWaterBombs() << "/上限:" << getMaxWaterBombs();
}

void Player::tryUseGlove() {
    if (!hasGlove || gloveCount <= 0 || !scene) return;

    // 根據方向決定 offset
    QPoint dirOffset;
    switch (getDirection()) {
        case Direction::Up:    dirOffset = QPoint(0, -1); break;
        case Direction::Down:  dirOffset = QPoint(0, 1); break;
        case Direction::Left:  dirOffset = QPoint(-1, 0); break;
        case Direction::Right: dirOffset = QPoint(1, 0); break;
        default: return;
    }

    QPoint from = getNearestGridPos();
    QPoint landing = from + dirOffset * 2; // 丟到第2格
    const int mapW = 11, mapH = 9;

    // wrap-around 處理
    landing.setX((landing.x() + mapW) % mapW);
    landing.setY((landing.y() + mapH) % mapH);

    // 若是磚塊就繼續 bounce
    while (scene->getMap(landing) == 1) {
        landing += dirOffset;
        landing.setX((landing.x() + mapW) % mapW);
        landing.setY((landing.y() + mapH) % mapH);
    }

    int range = 1 + powerPotionCount;

    qDebug() << "[Player] 使用 Glove 丟水球，落點:" << landing;
    qDebug() << "[player:tryUseGlove] range=" << range;

    scene->addWaterBomb(landing, this, range, true); // 第四個參數是 isGlove = true
    gloveCount--;

    if (gloveCount <= 0) {
        hasGlove = false;
        removeItem(ItemType::Glove);  // 可加可不加（視你想不想 UI 顯示）
        qDebug() << "[Player] Glove 用盡";
    }
}


QPoint Player::getStartPos(int currentWaveIndex){
    if (currentWaveIndex == 0){
        return QPoint(5,8);

    } else if (currentWaveIndex == 1){
        return QPoint(5,4);
    }
    else{
        return QPoint(5, 7);
    }
}

void Player::updateMoveSpeed() {
    moveSpeed = 75;  // 預設

    if (hasItem(ItemType::Turtle)) {
        moveSpeed *= 0.8;
    }

    if (hasItem(ItemType::SpeedShoes)) {
        moveSpeed *= 1.5;
    }

    qDebug() << "[Player] 更新速度為：" << moveSpeed;
}

void Player::onTurtleBreak() {
    removeItem(ItemType::Turtle);
    updateMoveSpeed();
    setStateStanding();
    setGridAlignedScreenPos(getNearestGridPos());

    // 開啟無敵
    invincible = true;
    invincibleFrameCounter = 0;

    if (!invincibleTimer) {
        invincibleTimer = new QTimer(this);
        invincibleTimer->setSingleShot(true);
        connect(invincibleTimer, &QTimer::timeout, this, [this]() {
            invincible = false;
            qDebug() << "[Player] 無敵結束 (來自 turtle)";
            setGridAlignedScreenPos(getStartPos(currentWaveIndex));
        });
    }
    invincibleTimer->start(2000);
    qDebug() << "[Player] onTurtleBreak: 移除 turtle + 進入無敵";
}

void Player::setGridAlignedScreenPos(QPoint gridPos) {
    QString key = getFrameKey();
    QPixmap sprite = SpriteSheetManager::instance().getFrame(key);
    int displayHeight = hasItem(ItemType::Turtle)? 79 : 56; // 預設

    if (!sprite.isNull()) {
        int displayWidth = 50;
        displayHeight = sprite.height() * displayWidth / sprite.width();
    }

    int x = gridPos.x() * 50;
    int y = gridPos.y() * 50 + 50 - displayHeight;
    gridPos = QPoint(x, y);
    screenPos = QPointF(x, y);
}

void Player::tryPushBrick(Direction dir) {
    if (!scene) return;

    QPoint currentGrid = getNearestGridPos();
    QPoint dirOffset;
    switch (dir) {
        case Direction::Up:    dirOffset = QPoint(0, -1); break;
        case Direction::Down:  dirOffset = QPoint(0,  1); break;
        case Direction::Left:  dirOffset = QPoint(-1, 0); break;
        case Direction::Right: dirOffset = QPoint( 1, 0); break;
        default: return;
    }

    QPoint target = currentGrid + dirOffset;
    QPoint behindTarget = target + dirOffset;

    // 若 target 為可推磚，且 behindTarget 是空的
    if (scene->getMap(target) == 1 && scene->getMap(behindTarget) == 0) {
        QRect behindRect(behindTarget.x() * 50, behindTarget.y() * 50, 50, 50);

        if (!scene->checkCollision(behindRect)) {
            // ✅ 更新地圖資訊
            scene->setMap(target, 0);         // 原本位置清空
            scene->setMap(behindTarget, 1);   // 新位置設為磚塊

            // ✅ 同時將 player 向該方向移動
            QPointF stepOffset(dirOffset.x() * 50.0, dirOffset.y() * 50.0);
            screenPos += stepOffset;

            qDebug() << "[Player] 推磚成功，方向:" << static_cast<int>(dir);
            return;
        }
    }

    qDebug() << "[Player] 推磚失敗，前方:" << target << "後方:" << behindTarget;
}
