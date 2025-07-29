#pragma once

#include "IGameController.h"
#include "BattleScene.h"
#include "Robot.h"
#include "MapLoader.h"
#include <QObject>

class GameControllerMode1 : public QObject, public IGameController {
    Q_OBJECT

public:
    GameControllerMode1();
    ~GameControllerMode1();

    void initialize(BattleScene* scene) override;
    void update(float delta) override;

    Player* getPlayer() const override;
    void setPlayer(Player* p) override { player = p; }
    QVector<Robot*> getRobots() const override;
    GameMode getMode() const override;

    void reset() override;

private:
    BattleScene* scene = nullptr;
    QVector<QVector<int>> map;
    Player* player = nullptr;
    QVector<Robot*> robots;
};
