#include "BattleScene.h"
#include "GameControllerMode1.h"
#include "SpriteSheetManager.h"
#include "Robot.h"

#include <QPainter>
#include <QDebug>

BattleScene::BattleScene(QWidget *parent)
    : QWidget(parent) {}

QPoint BattleScene::getPlayerGridPos() const {
    if (player)
        return player->getGridPos();
    return QPoint(-1, -1);
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

void BattleScene::setPlayer(Player* p) {
    player = p;
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
    updateTimer.start(16);  // 每 16ms 執行一次
}

void BattleScene::paintEvent(QPaintEvent*) {
    QPainter painter(this);


    int cellSize = 50;
    SpriteSheetManager& sheet = SpriteSheetManager::instance();
    paintMap(painter,sheet,cellSize);
    paintPlayer(painter,sheet);
    paintMonsters(painter);     // ✅ 畫出 monster
    paintRobot(painter);        // ✅ 畫出 robot
    paintWaterBombs(painter);   // ✅ 畫出水球
    paintExplosions(painter);   // ✅ 畫出水球爆炸
    paintUI(painter);

}


void BattleScene::addMonster(Monster* m) {
    monsters.push_back(m);
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

void BattleScene::addPlayer(Player* p, const QPoint& pos) {
    player = p;
    player->setGridPos(pos);
    qDebug() << "[BattleScene] add player at" << pos;
    update();
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

Player* BattleScene::getPlayer() const {
    return player;
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
    } else if (Octopus* o = qobject_cast<Octopus*>(item)) {
        octopi.removeAll(o);
    }

    item->deleteLater();  // 安全移除
}


void BattleScene::clearScene() {
    // 清除怪物與章魚
    for (Monster* m : monsters)
        delete m;
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
    if (player) {
        delete player;
        player = nullptr;
    }

    if (robot) {
        delete robot;
        robot = nullptr;
    }

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

    Player* player = controller->getPlayer();
    QPointF pos = player->getScreenPos();

    QString directionStr;
    switch (player->getDirection()) {
        case Direction::Down:  directionStr = "down"; break;
        case Direction::Up:    directionStr = "up"; break;
        case Direction::Left:  directionStr = "left"; break;
        case Direction::Right: directionStr = "right"; break;
        default: directionStr = "down"; break;
    }

    QString playerKey = QString("P_stand_%1_1").arg(directionStr);
    QPixmap playerImg = sheet.getFrame(playerKey);
    if (playerImg.isNull()) {
        painter.setBrush(Qt::red);
        painter.drawEllipse(QRect(pos.x(), pos.y(), 50, 50));
        return;
    }

    // 等比縮放寬度為 50
    int displayWidth = 50;
    int displayHeight = playerImg.height() * displayWidth / playerImg.width();

    int offsetY = displayHeight - 50;
    QRect drawRect(pos.x(), pos.y() - offsetY, displayWidth, displayHeight);  // ⭐ 向上位移
    painter.drawPixmap(drawRect, playerImg);
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
