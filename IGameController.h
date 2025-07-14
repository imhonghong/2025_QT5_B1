#pragma once

#include <QVector>
#include <QPoint>
#include "Player.h"
#include "Robot.h"
#include "GameMode.h"

class BattleScene;
class IGameController {
public:
    virtual ~IGameController() {}

    virtual void initialize(BattleScene* scene) = 0;
    virtual void update(float delta) = 0;

    virtual Player* getPlayer() const = 0;
    virtual QVector<Robot*> getRobots() const = 0;
    virtual GameMode getMode() const = 0;

    virtual void reset() = 0;
};
