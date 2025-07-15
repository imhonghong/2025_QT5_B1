#pragma once

#include <QObject>
#include <QPoint>
#include <QElapsedTimer>

class WaterBomb : public QObject {
    Q_OBJECT
public:
    WaterBomb(QPoint gridPos, QObject* parent = nullptr);

    QPoint getGridPos() const;
    QPixmap getCurrentPixmap() const;

    bool getHasExploded() const;
    void explode();

signals:
    void exploded(QPoint center);  // 可連接後續爆炸影響

public slots:
    void tick(); // 每次更新或畫面重繪時呼叫

private:
    QPoint gridPos;
    QElapsedTimer timer;
    int currentFrame = 1; // 1~4 對應 WB_1 ~ WB_4
    bool hasExploded = false;
};
