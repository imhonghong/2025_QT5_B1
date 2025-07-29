#pragma once

#include <QObject>
#include <QElapsedTimer>
#include <QPoint>
#include <QRect>
#include "Direction.h"

class BattleScene;
class OctopusBall : public QObject {
    Q_OBJECT

public:
    OctopusBall(QPoint start, Direction dir, BattleScene* scene);
    ~OctopusBall();

    void tick();
    void explode();
    bool isTimeUp() const;

    void moveForward();
    QPoint getGridPos() const;
    QRect getBoundingBox() const;
    QPixmap getCurrentPixmap() const;

signals:
    void requestExplosion(OctopusBall* ball);

private:
    BattleScene* scene = nullptr;
    QPoint gridPos;
    Direction direction;
    QElapsedTimer timer;
    int currentFrame = 1;
    bool hasExploded = false;

    void updateFrame();
};
