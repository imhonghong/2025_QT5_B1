#pragma once
#include <QPoint>
#include <QPointF>

class Character {
public:
    virtual ~Character() = default;

    // 畫面位置（用於平滑移動）
    QPointF getScreenPos() const { return screenPos; }
    void setScreenPos(const QPointF& p) { screenPos = p; }

    // 格子座標（地圖邏輯、判定使用）
    QPoint getGridPos() const { return gridPos; }
    virtual void setGridPos(const QPoint& p) {
        gridPos = p;
        screenPos = QPointF(p.x() * 50, p.y() * 50);  // 自動更新畫面位置
    }

    void setDirection(int d) { direction = d; }
    int getDirection() const { return direction; }

protected:
    QPoint gridPos = {-1, -1};
    QPointF screenPos = {0, 0};
    int direction = 0;
};
