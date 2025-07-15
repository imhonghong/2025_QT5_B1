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
    mapData = map;
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

    // ✅ 新增以下
    connect(&updateTimer, &QTimer::timeout, this, [=](){
        if (controller) controller->update(0.016f);  // 假設約 60fps
        update();  // 強制重繪畫面
    });
    updateTimer.start(16);  // 每 16ms 執行一次
}

void BattleScene::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    int cellSize = 50;
    SpriteSheetManager& sheet = SpriteSheetManager::instance();

    for (int y = 0; y < mapData.size(); ++y) {
        for (int x = 0; x < mapData[y].size(); ++x) {
            int type = mapData[y][x];
            QRect rect(x * cellSize, y * cellSize, cellSize, cellSize);

            QString frameKey;
            switch (type) {
                case 0: frameKey = "brick_0"; break;  // floor
                case 1: frameKey = "brick_1"; break;  // wall
                case 2: frameKey = "brick_2"; break;  // hard block
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

    // 玩家角色圖
    if (controller && controller->getPlayer()) {
        player = controller->getPlayer();
        QPointF pos = player->getScreenPos();
        QRect rect(pos.x(), pos.y(), cellSize, cellSize);

        QString directionStr;
        switch (player->getDirection()) {
            case Direction::Down: directionStr = "down"; break;
            case Direction::Up: directionStr = "up"; break;
            case Direction::Left: directionStr = "left"; break;
            case Direction::Right: directionStr = "right"; break;
            default: directionStr = "down"; break;
        }

        QString playerKey = QString("P_stand_%1_1").arg(directionStr);
        QPixmap playerImg = sheet.getFrame(playerKey);
        if (!playerImg.isNull()) {
            painter.drawPixmap(rect, playerImg);
        } else {
            painter.setBrush(Qt::red);
            painter.drawEllipse(rect);
        }
    }
    if (controller) {
        for (Robot* r : controller->getRobots()) {
            QPointF pos = r->getScreenPos();
            QRect rect(pos.x(), pos.y(), cellSize, cellSize);
            QString key = r->getFrameKey();
            QPixmap sprite = SpriteSheetManager::instance().getFrame(key);
            painter.drawPixmap(rect, sprite);
        }
    }
    // ✅ 畫出水球
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

    // ✅ 畫出水球爆炸
    for (int i = explosions.size() - 1; i >= 0; --i) {
        explosions[i]->draw(painter);
        explosions[i]->tick();
        if (explosions[i]->isExpired()) {
            delete explosions[i];
            explosions.removeAt(i);
        }
    }

}


void BattleScene::addMonster(Monster* m) {
    monsters.append(m);
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
