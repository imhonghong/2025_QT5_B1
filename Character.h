#pragma once
#include <QPointF>

class Character {
public:
    virtual ~Character() = default;

    void setPosition(float x, float y) { position = QPointF(x, y); }
    QPointF getPosition() const { return position; }

    void setDirection(int dir) { direction = dir; }
    int getDirection() const { return direction; }

protected:
    QPointF position;
    int direction = 0; // 0=down, 1=up, 2=left, 3=right
};
