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
    void setScene(BattleScene* s);
    void setPlayer(Player* p) override { player = p; }
    int getCurrentWave() const { return currentWave; }

    void nextWave();       // wave0 → wave1
    void clearPlayer();    // 安全刪除 player
    void clearItems();     // 安全刪除 item（或交由 scene 處理）


private:
    BattleScene* scene = nullptr;
    Player* player = nullptr;
    QVector<Monster*> monsters;
    int currentWave = 2;

    void loadWave(int waveIndex);  // 載入地圖、怪物、玩家
    void clearMonsters();
    void checkWaveCleared();

    QVector<QVector<int>> map;
    QPoint initWave0();
    QPoint initWave1();
    QPoint initWave2();

};
