#pragma once

#include <QObject>
#include "Character.h"

class IGameController;

class Player : public Character {
public:
    Player();

    void setController(IGameController* controller);
    void update(float delta);

private:
    IGameController* controller = nullptr;
};
