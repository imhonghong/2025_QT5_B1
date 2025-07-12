#include "BattleScene.h"
#include "GameControllerMode1.h"
#include "SpriteSheetManager.h"
#include "Robot.h"

#include <QPainter>
#include <QDebug>

BattleScene::BattleScene(QWidget *parent)
    : QWidget(parent) {}

void BattleScene::setMap(const QVector<QVector<int>>& map) {
    mapData = map;
}

void BattleScene::setPlayer(Player* p) {
    player = p;
}

void BattleScene::setController(IGameController* c) {
    if (controller) delete controller;
    controller = c;
    if (controller) controller->initialize(this);
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
            case 0: directionStr = "down"; break;
            case 1: directionStr = "up"; break;
            case 2: directionStr = "left"; break;
            case 3: directionStr = "right"; break;
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
            QString key = r->getCurrentSprite();
            QPixmap sprite = SpriteSheetManager::instance().getFrame(key);
            painter.drawPixmap(rect, sprite);
        }
    }

}


void BattleScene::addMonster(Monster* m) {
    monsters.append(m);
    update();
}
