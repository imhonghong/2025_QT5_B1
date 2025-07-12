#include "GameController.h"
#include "BattleScene.h"
#include "MapLoader.h"
#include <QDebug>

GameController::GameController() {}

void GameController::initialize(GameMode mode, BattleScene* scene) {
    this->scene = scene;
    this->currentMode = mode;

    mapData = MapLoader::loadMap(":/map/map.txt");
    scene->setMap(mapData);

    if (mode == GameMode::Mode1) {
        for (int y = 0; y < mapData.size(); ++y) {
            for (int x = 0; x < mapData[y].size(); ++x) {
                int type = mapData[y][x];
                if (type == 4) {
                    player = new Player();
                    player->setPosition(x, y);
                    player->setDirection(0);
                    scene->setPlayer(player);
                }
                if (type == 5) {
                    /*
                    Robot* robot = new Robot();
                    robot->setPosition(x, y);
                    scene->addRobot(robot);
                    */
                }
            }
        }
    }
    else if (mode == GameMode::Mode2) {
        // player 固定在 (1,1)
        player = new Player();
        player->setPosition(1, 1);
        player->setDirection(0);
        scene->setPlayer(player);

        // monster 角落生成
        /*
        Monster* m1 = new Monster(); m1->setPosition(1, 9);  m1->setMovePattern("clockwise");
        Monster* m2 = new Monster(); m2->setPosition(9, 1);  m2->setMovePattern("counterclockwise");
        Monster* m3 = new Monster(); m3->setPosition(9, 9);  m3->setMovePattern("tracker");
        scene->addMonster(m1);
        scene->addMonster(m2);
        scene->addMonster(m3);
        */
    }
}


void GameController::update(float delta) {
    if (player) {
        player->update(delta);
    }
}

Player* GameController::getPlayer() {
    return player;
}
