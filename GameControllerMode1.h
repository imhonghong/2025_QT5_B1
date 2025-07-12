#pragma once

#include "IGameController.h"
#include "BattleScene.h"
#include "MapLoader.h"

class GameControllerMode1 : public IGameController {
public:
    GameControllerMode1();
    ~GameControllerMode1();

    void initialize(BattleScene* scene) override;
    void update(float delta) override;

    Player* getPlayer() const override;
    QVector<Robot*> getRobots() const override;
    GameMode getMode() const override;

    void reset() override;

private:
    BattleScene* scene = nullptr;
    QVector<QVector<int>> map;
    Player* player = nullptr;
    QVector<Robot*> robots;
};
