#pragma once

#include <QObject>
#include <QPointF>
#include "GameMode.h"

class GameController;

class Player {
public:
    Player();

    void setController(GameController* controller);
    void setPosition(float x, float y);
    void setDirection(int dir); // 0:up, 1:down, 2:left, 3:right

    QPointF getPosition() const;
    int getDirection() const;

    void update(float delta); // update movement logic

private:
    GameController* controller = nullptr;
    QPointF position; // float position for smooth movement
    int direction = 0;
};
