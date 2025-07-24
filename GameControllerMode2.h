#pragma once

#include "IGameController.h"
#include "BattleScene.h"
#include "Player.h"
#include "Monster.h"
#include <QObject>
#include <QVector>

class GameControllerMode2 : public QObject, public IGameController {
    Q_OBJECT
public:
    void initialize(BattleScene* scene) override;
    void update(float delta) override;
    Player* getPlayer() const override;
    QVector<Robot*> getRobots() const override { return {}; }
    GameMode getMode() const override { return GameMode::Mode2; }
    void reset() override;

private:
    BattleScene* scene = nullptr;
    Player* player = nullptr;
    QVector<Monster*> monsters;
    int currentWave = 0;

    void loadWave(int waveIndex);  // 載入地圖、怪物、玩家
    void clearMonsters();
    void checkWaveCleared();

    QVector<QVector<int>> map;
    QPoint initWave0();
    QPoint initWave1();

};
