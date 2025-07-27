#include "BattleScene.h"
#include "GameControllerMode1.h"
#include "SpriteSheetManager.h"
#include "Robot.h"
#include "Item.h"

#include <QPainter>
#include <QDebug>
#include <QGraphicsScene>

BattleScene::BattleScene(QWidget *parent)
    : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);  // 允許接受鍵盤事件
    setFocus();                       // 主動取得焦點
    qDebug() << "[BattleScene] Focus set for key input.";
}

QPoint BattleScene::getPlayerGridPos() const {
    Player* p = getPlayer();
    return p ? p->getGridPos() : QPoint(-1, -1);
}

QVector<QVector<int>> BattleScene::getCurrentMap() const {
    return mapData;
}

int BattleScene::getMap(const QPoint& p) const {
    if (p.y() < 0 || p.y() >= mapData.size() ||
        p.x() < 0 || p.x() >= mapData[0].size())
        return -1;
    return mapData[p.y()][p.x()];
}

void BattleScene::setMap(const QPoint& p, int value) {
    if (p.y() < 0 || p.y() >= mapData.size() ||
        p.x() < 0 || p.x() >= mapData[0].size())
        return;
    mapData[p.y()][p.x()] = value;
}

void BattleScene::setMap(const QVector<QVector<int>>& map) {
    if (map.isEmpty() || map[0].isEmpty()) return;
    mapData = map;
    update();
}


void BattleScene::setRobot(Robot* r) {
    robot = r;
}

void BattleScene::setController(IGameController* c) {
    if (controller) delete controller;
    controller = c;
    if (controller) controller->initialize(this);

    connect(&updateTimer, &QTimer::timeout, this, [=](){
        if (!isPaused && controller) {
            controller->update(0.016f);
            update();
        }
    });
    setFocus();
    updateTimer.start(16);  // 每 16ms 執行一次
}

void BattleScene::bindPlayerForMode1(Player* p) {
    // 僅允許 Mode1 設定 player
    if (controller && controller->getMode() == GameMode::Mode1) {
        qDebug() << "[BattleScene] Set player for Mode1";
        p->setScene(this); // ⭐ 關鍵：設定 scene
        p->setController(controller);
    } else {
        qWarning() << "[BattleScene] setPlayer() 只允許在 Mode1 呼叫";
    }
}

void BattleScene::paintEvent(QPaintEvent*) {
    QPainter painter(this);


    int cellSize = 50;
    SpriteSheetManager& sheet = SpriteSheetManager::instance();
    paintMap(painter,sheet,cellSize);
    paintPlayer(painter,sheet);
    paintMonsters(painter);     // ✅ 畫出 monster
    paintRobot(painter);        // ✅ 畫出 robot
    paintItems(painter);
    paintWaterBombs(painter);   // ✅ 畫出水球
    paintExplosions(painter);   // ✅ 畫出水球爆炸
    paintUI(painter);


}


void BattleScene::addMonster(Monster* m) {
    monsters.push_back(m);
    m->setScene(this);
    connect(m, &Monster::requestDelete, this, &BattleScene::removeItem);
    update();
}

void BattleScene::addWaterBomb(WaterBomb* bomb) {
    waterBombs.append(bomb);
    qDebug() << "[BattleScene] Add WaterBomb at" << bomb->getGridPos(); // ✅ 加上
    connect(bomb, &WaterBomb::exploded, this, [=](QPoint center){
        qDebug() << "[BattleScene] Bomb exploded at" << center;
        explosions.append(new Explosion(center, this));
    });
    update();
}


void BattleScene::addWaterBomb(QPoint gridPos, Player* owner, int range, bool isGlove) {
    if (!isGlove) {
        for (WaterBomb* bomb : waterBombs) {
            if (bomb->getGridPos() == gridPos) {
                qDebug() << "[BattleScene] 該格已有水球，無法放置";
                return;
            }
        }
    }

    int tile = getMap(gridPos);
    if (tile == 1 || tile == 2) {
        qDebug() << "[BattleScene] 該格為牆或磚，無法放置";
        return;
    }

    // ✅ 修正：使用傳入的 range 參數
    auto* bomb = new WaterBomb(gridPos, this, owner, range);
    waterBombs.append(bomb);

    // ✅ 修正：在 lambda 中捕獲 bomb 並使用其 range
    connect(bomb, &WaterBomb::exploded, this, [=](QPoint center){
        qDebug() << "[BattleScene] Bomb exploded at" << center << "with range" << bomb->getRange();
        explosions.append(new Explosion(center, this, bomb->getRange()));  // ← 使用正確的 range
    });

    update();
    qDebug() << "[BattleScene] 放置水球於:" << gridPos << " range=" << range;
}



bool BattleScene::hasWaterBomb(const QPoint& gridPos) const {
    for (WaterBomb* b : waterBombs) {
        if (b->getGridPos() == gridPos)
            return true;
    }
    return false;
}

int BattleScene::getWaterBombCount(Player* owner) const {
    int count = 0;
    for (WaterBomb* b : waterBombs) {
        if (b->parent() == owner)
            ++count;
    }
    return count;
}

void BattleScene::addPlayer(Player* p, const QPoint& pos) {
    if (p) {
        p->setScene(this);  // 傳給 player battleScene
        p->setGridAlignedScreenPos(pos);
        qDebug() << "[BattleScene] add player at" << pos;
        update();
    }
}

void BattleScene::addBrick(const QPoint& pos, int type) {
    if (pos.y() >= 0 && pos.y() < mapData.size() &&
        pos.x() >= 0 && pos.x() < mapData[0].size()) {
        mapData[pos.y()][pos.x()] = type;
        update();
    }
}

const QVector<WaterBomb*>& BattleScene::getWaterBombs() const {
    return waterBombs;
}

Robot* BattleScene::getRobot() const {
    return robot;
}

const QVector<Monster*>& BattleScene::getMonsters() const {
    return monsters;
}

const QVector<Octopus*>& BattleScene::getOctopi() const {
    return octopi;
}

void BattleScene::removeItem(QObject* item) {
    if (!item) return;

    // 嘗試從場上移除怪物或章魚
    if (Monster* m = qobject_cast<Monster*>(item)) {
        monsters.removeAll(m);
        emit monsterRemoved(m);
    } else if (Octopus* o = qobject_cast<Octopus*>(item)) {
        octopi.removeAll(o);
    }

    item->deleteLater();  // 安全移除
    qDebug() << "[BattleScene] removeItem called on" << item;
}


void BattleScene::clearScene() {
    // 清除怪物與章魚
    for (Monster* m : monsters)
        removeItem(m);
    monsters.clear();

    for (Octopus* o : octopi)
        delete o;
    octopi.clear();

    // 清除水球與爆炸
    for (WaterBomb* b : waterBombs)
        delete b;
    waterBombs.clear();

    for (Explosion* e : explosions)
        delete e;
    explosions.clear();

    // 玩家、機器人等也一併清除（如果有的話）
    Player* p = getPlayer();
    if (p) {
        delete p; // 不設為 nullptr，因為你沒儲存這個指標
    }

    if (robot) {
        delete robot;
        robot = nullptr;
    }

    clearItems();
    update();
    // 清除地圖
    if (!mapData.isEmpty() && !mapData[0].isEmpty()) {
        int rows = mapData.size();
        int cols = mapData[0].size();
        mapData = QVector<QVector<int>>(rows, QVector<int>(cols, 0));
    }
}

void BattleScene::paintMap(QPainter& painter, SpriteSheetManager& sheet, int cellSize) {
    for (int y = 0; y < mapData.size(); ++y) {
        for (int x = 0; x < mapData[y].size(); ++x) {
            int type = mapData[y][x];
            QRect rect(x * cellSize, y * cellSize, cellSize, cellSize);

            QString frameKey;
            switch (type) {
                case 0: frameKey = "brick_0"; break;
                case 1: frameKey = "brick_1"; break;
                case 2: frameKey = "brick_2"; break;
                case 3: frameKey = ((x + y) % 2 == 0) ? "brick_3_0" : "brick_3_1"; break;
                case 6: frameKey = "brick_6"; break;
                case 7: frameKey = "brick_7"; break;
                default: frameKey = "brick_0"; break;
            }

            QPixmap sprite = sheet.getFrame(frameKey);
            if (!sprite.isNull()) {
                painter.drawPixmap(rect, sprite);
            } else {
                painter.fillRect(rect, Qt::gray);
            }
        }
    }
}

void BattleScene::paintPlayer(QPainter& painter, SpriteSheetManager& sheet) {
    if (!controller || !controller->getPlayer()) return;

    Player* p = controller ? controller->getPlayer() : nullptr;
    if (!p) return;

    QString frameKey = p->getFrameKey();
    if (frameKey.isEmpty()) {
        return;
    }

    QPointF pos = p->getScreenPos();

    QString directionStr;
    switch (p->getDirection()) {
        case Direction::Down:  directionStr = "down"; break;
        case Direction::Up:    directionStr = "up"; break;
        case Direction::Left:  directionStr = "left"; break;
        case Direction::Right: directionStr = "right"; break;
        default: directionStr = "down"; break;
    }

    QPixmap playerImg = SpriteSheetManager::instance().getFrame(p->getFrameKey());


    if (playerImg.isNull()) {
        painter.setBrush(Qt::red);
        painter.drawEllipse(QRect(pos.x(), pos.y(), 46, 46));
        return;
    }

    QRect drawRect(pos.x(), pos.y(), 50, playerImg.height());
    painter.drawPixmap(drawRect, playerImg);

    // 顯示碰撞框
    if (Player* p = getPlayer()) {
        QRect rect = p->getCollisionBox();
        painter.setPen(QPen(Qt::red, 2));
        painter.drawRect(rect);
    }
}


void BattleScene::paintMonsters(QPainter& painter){
    for (const auto& m : monsters) {
        QPixmap sprite = SpriteSheetManager::instance().getFrame(m->getFrameKey());
        painter.drawPixmap(QRect(m->getScreenPos().toPoint(), QSize(50, 50)), sprite);
    }
}
void BattleScene::paintWaterBombs(QPainter& painter){
    for (int i = waterBombs.size() - 1; i >= 0; --i) {
        WaterBomb* b = waterBombs[i];
        b->tick();
        if (b->getCurrentPixmap().isNull()) {
            delete b;
            waterBombs.removeAt(i);
            continue;
        }
        QPixmap pix = b->getCurrentPixmap();
        QPointF pos = QPointF(b->getGridPos().x() * 50, b->getGridPos().y() * 50);
        painter.drawPixmap(QRect(pos.x(), pos.y(), 50, 50), pix);
    }
}
void BattleScene::paintExplosions(QPainter& painter){
    for (int i = explosions.size() - 1; i >= 0; --i) {
        explosions[i]->draw(painter);
        explosions[i]->tick();
        if (explosions[i]->isExpired()) {
            delete explosions[i];
            explosions.removeAt(i);
        }
    }
}

void BattleScene::paintRobot(QPainter& painter){
    if (controller) {
        for (Robot* r : controller->getRobots()) {
            QPointF pos = r->getScreenPos();
            QRect rect(pos.x(), pos.y(), 50, 50);
            QString key = r->getFrameKey();
            QPixmap sprite = SpriteSheetManager::instance().getFrame(key);
            painter.drawPixmap(rect, sprite);
        }
    }
}

void BattleScene::paintUI(QPainter& painter) {
    SpriteSheetManager& sheet = SpriteSheetManager::instance();
    painter.drawPixmap(0, 450, 550, 100, sheet.getFrame("ui"));  // 背景

    // 畫兩個按鈕
    //painter.drawPixmap(QRect(pos.x(), pos.y(), 50, 50), pix);
    painter.drawPixmap(pauseButtonRect, sheet.getFrame("pause"));
    painter.drawPixmap(homeButtonRect, sheet.getFrame("home"));

    // 顯示玩家 HP（左上）
    if (controller && controller->getPlayer()) {
        Player* player = controller->getPlayer();
        int hp = player->getHp();
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 18));
        painter.drawText(75, 525, QString("%1").arg(hp));
    }

    if (controller && controller->getMode() == GameMode::Mode1) {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(200, 480, QString("Step: %1").arg(stepCount));
    }

    if (controller && controller->getMode() == GameMode::Mode2) {
        Player* player = controller->getPlayer();
        if (player) {
            int needleCount = player->getNeedleCount();  // 你需新增這個 getter
            painter.setPen(Qt::black);
            painter.setFont(QFont("Arial", 12));
            painter.drawText(455, 535, QString("x%1").arg(needleCount));
        }
    }
}

void BattleScene::paintItems(QPainter &painter){
    for (Item* item : items) {
        QPoint pos = item->getScreenPos();
        QString key = item->getName();
        QPixmap pix = SpriteSheetManager::instance().getFrame(key);
        painter.drawPixmap(pos.x(), pos.y(), 50, 50, pix);
    }
}

void BattleScene::mousePressEvent(QMouseEvent* event) {
    QPoint pos = event->pos();

    if (pauseButtonRect.contains(pos)) {
        emit pauseRequested();  // 你可以定義一個 signal
        return;
    }

    if (homeButtonRect.contains(pos)) {
        emit returnToMainMenu();  // 你可以定義一個 signal
        return;
    }

    QWidget::mousePressEvent(event);
}

void BattleScene::togglePause() {
    isPaused = !isPaused;
}

void BattleScene::keyPressEvent(QKeyEvent* event) {
    if (isPaused) return;

    Player* player = getPlayer();  // 已定義於 header
    if (!player) {
        qDebug() << "[Scene] player is nullptr";
        return;
    }

    switch (event->key()) {
    case Qt::Key_Up:
        player->addMoveKey(Direction::Up);
        break;
    case Qt::Key_Down:
        player->addMoveKey(Direction::Down);
        break;
    case Qt::Key_Left:
        player->addMoveKey(Direction::Left);
        break;
    case Qt::Key_Right:
        player->addMoveKey(Direction::Right);
        break;
    case Qt::Key_Space:{
           if (player->hasItem(ItemType::Glove)) {
               player->tryUseGlove();  // 使用手套邏輯
           } else {
               player->tryPlaceWaterBomb();  // 一般放置水球
           }
        }
        break;

    case Qt::Key_Shift:
        if (player->getIsTrapped()) {
            player->tryUseNeedle();
            return; // 防止再觸發方向鍵
            }
    break;
    }
}

void BattleScene::keyReleaseEvent(QKeyEvent* event) {
    Player* p = getPlayer();
    if (!p) return;
    switch (event->key()) {
        case Qt::Key_Up:    p->removeMoveKey(Direction::Up); break;
        case Qt::Key_Down:  p->removeMoveKey(Direction::Down); break;
        case Qt::Key_Left:  p->removeMoveKey(Direction::Left); break;
        case Qt::Key_Right: p->removeMoveKey(Direction::Right); break;
    }
}

bool BattleScene::checkCollision(const QRect& box) const {
    for (const Monster* m : monsters) {
        if (box.intersects(m->getCollisionBox())) return true;
    }

    for (int y = 0; y < mapData.size(); ++y) {
        for (int x = 0; x < mapData[y].size(); ++x) {
            int type = mapData[y][x];
            if (type == 1 || type == 2 || type == 3 || type == 6 || type == 7) { // 不可穿越磚
                QRect brickBox(x * 50, y * 50, 50, 50);
                if (box.intersects(brickBox)) return true;
            }
        }
    }

    // 檢查是否撞到未爆炸的水球
    Player* player = getPlayer(); // 對 Mode2 有效

    for (WaterBomb* bomb : waterBombs) {
        QRect bombRect(bomb->getGridPos() * 50, QSize(50, 50));

        // 如果還在自己剛剛放的水球內，且未完全離開，不阻擋
        if (bomb->getOwner() == player) {
            if (!bomb->hasPlayerLeft()) {
                // 玩家還沒完全離開自己剛放的水球，不視為阻擋
                continue;
            }
        }

        if (box.intersects(bombRect)) {
            qDebug() << "[Collision] 撞到水球 at" << bomb->getGridPos();
            return true;
        }
    }

    return false;
}

bool BattleScene::checkCollisionExcludingMonsters(const QRect& box) const {
    for (int y = 0; y < mapData.size(); ++y) {
        for (int x = 0; x < mapData[y].size(); ++x) {
            if (mapData[y][x] == 1 || mapData[y][x] == 2) {
                QRect rect(x * 50, y * 50, 50, 50);
                if (rect.intersects(box)) return true;
            }
        }
    }

    for (auto* bomb : waterBombs) {
        QRect bombRect(bomb->getGridPos() * 50, QSize(50, 50));
        if (bombRect.intersects(box)) return true;
    }

    return false;
}


void BattleScene::addItem(Item* item) {
    // 安全設圖
    QString key = item->getName();
    QPixmap pix = SpriteSheetManager::instance().getFrame(key);
    if (pix.isNull()) {
        qWarning() << "[BattleScene] 無法載入圖片:" << key;
    } else {
        item->setPixmap(pix);
    }

    // 移除遞迴呼叫，直接加入到 items 列表
    items.push_back(item);

    // 觸發重繪
    update();

    qDebug() << "[BattleScene] 成功添加 item:" << key << "at" << item->getGridPos();
}

void BattleScene::clearItems() {
    for (Item* item : items) {
        removeItem(item);  // ✅ 從 scene 中移除圖示
        item->deleteLater();       // ✅ 刪除物件
    }
    items.clear();         // ✅ 清空 list
}

QVector<Item*>& BattleScene::getItems() {
    return items;
}

bool BattleScene::tryPushBrick(const QRect& playerBox, Direction dir) {
    QPoint dirOffset;
    switch (dir) {
        case Direction::Up: dirOffset = QPoint(0, -1); break;
        case Direction::Down: dirOffset = QPoint(0, 1); break;
        case Direction::Left: dirOffset = QPoint(-1, 0); break;
        case Direction::Right: dirOffset = QPoint(1, 0); break;
        default: return false;
    }

    // 取得碰撞框中心對應的格子
    QPoint center = playerBox.center() / 50;
    QPoint target = center + dirOffset;
    QPoint behind = target + dirOffset;

    // 檢查是否為可推磚
    if (getMap(target) != 1)
        return false;

    // 後方必須是空格
    if (getMap(behind) != 0)
        return false;

    QRect behindBox(behind * 50, QSize(50, 50));
    if (checkCollision(behindBox))
        return false;

    // ✅ 推動成功：更新 mapData 並觸發更新
    setMap(target, 0);
    setMap(behind, 1);
    update();  // 重繪畫面
    return true;
}
