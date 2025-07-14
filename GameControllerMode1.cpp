#include "GameControllerMode1.h"
#include "Robot.h"
#include <QDebug>

GameControllerMode1::GameControllerMode1() {}

GameControllerMode1::~GameControllerMode1() {
    reset();
}

void GameControllerMode1::initialize(BattleScene* scene) {
    reset();
    this->scene = scene;

    map = MapLoader::loadMap(":/map/map.txt");
    scene->setMap(map);

    // ⬇️ 第一階段：初始化 player
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == 4) {
                player = new Player();
                player->setGridPos(QPoint(x, y));
                player->setDirection(0);
                scene->setPlayer(player);
                break;  // 一次只有一位 player，找到就離開
            }
        }
        if (player) break;
    }

    // ⬇️ 第二階段：初始化 robots（此時 player 一定存在）
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == 5) {
                Robot* robot = new Robot();
                robot->setScene(scene);
                robot->setGridPos(QPoint(x, y));
                robot->generateTestPlan();
                //robot->generatePlan(map, player->getGridPos());
                robots.push_back(robot);
                qDebug() << "[Robot] 初始位置:" << x << y;
            }
        }
    }

    qDebug() << "[GameControllerMode1] Initialized with player and" << robots.size() << "robot(s)";
}

void GameControllerMode1::update(float delta) {
    for (Robot* r : robots) {
        if (!r->isFinished()) {
            r->advanceStep();
        }
    }
}

Player* GameControllerMode1::getPlayer() const {
    return player;
}

QVector<Robot*> GameControllerMode1::getRobots() const {
    return robots;
}

GameMode GameControllerMode1::getMode() const {
    return GameMode::Mode1;
}

void GameControllerMode1::reset() {
    for (Robot* r : robots) delete r;
    robots.clear();

    if (player) {
        delete player;
        player = nullptr;
    }

    map.clear();
    scene = nullptr;
}
