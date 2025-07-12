#pragma once

#include "GameMode.h"
#include "Player.h"
#include <QVector>

class BattleScene;

class GameController {
public:
    GameController();
    void initialize(GameMode mode, BattleScene* scene);
    void update(float delta); // 更新所有遊戲邏輯

    Player* getPlayer();

private:
    GameMode currentMode;
    BattleScene* scene = nullptr;
    Player* player = nullptr;
    QVector<QVector<int>> mapData;
};
