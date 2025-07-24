#pragma once

#include <QObject>
#include <QPoint>
#include <QElapsedTimer>

#include "Player.h"


class WaterBomb : public QObject {
    Q_OBJECT
public:
    WaterBomb(QPoint gridPos, QObject* parent = nullptr);
    WaterBomb(QPoint gridPos, BattleScene* scene, Player* owner);

    QPoint getGridPos() const;
    QPixmap getCurrentPixmap() const;

    bool getHasExploded() const;
    void explode();

    Player* getOwner() const { return owner; }
    void setPlayerHasLeft(bool val) { playerHasLeft = val; }
    bool hasPlayerLeft() const { return playerHasLeft; }

signals:
    void exploded(QPoint center);  // 可連接後續爆炸影響

public slots:
    void tick(); // 每次更新或畫面重繪時呼叫

private:
    QPoint gridPos;
    QElapsedTimer timer;
    int currentFrame = 1; // 1~4 對應 WB_1 ~ WB_4
    bool hasExploded = false;

    Player* owner;
    bool playerHasLeft = false;
};
