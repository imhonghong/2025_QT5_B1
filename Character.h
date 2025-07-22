#pragma once
#include <QPoint>
#include <QPointF>
#include <QString>
#include <QRect>
#include <QSize>
#include <QPixmap>
#include "SpriteSheetManager.h"

enum class Direction {
    Down,
    Left,
    Right,
    Up
};

class Character {
public:
    virtual ~Character() = default;

    // --- 畫面位置（用於平滑移動） ---
    QPointF getScreenPos() const { return screenPos; }
    void setScreenPos(const QPointF& p) { screenPos = p; }

    // --- 格子座標（地圖邏輯、判定使用） ---
    QPoint getGridPos() const { return gridPos; }
    virtual void setGridPos(const QPoint& p) {
        gridPos = p;
        screenPos = QPointF(p.x() * 50, p.y() * 50);  // 自動更新畫面位置
    }

    // --- 方向（整數或QPoint皆可） ---
    void setDirection(Direction d) { direction = d; }
    Direction getDirection() const { return direction; }

    // --- HP 控制 ---
    virtual void setHp(int h) { hp = h; }
    virtual int getHp() const { return hp; }
    virtual bool isDead() const { return hp <= 0; }

    virtual void takeDamage(int dmg = 1) {
        hp -= dmg;
        if (hp <= 0) onDie();
    }

    virtual void onDie() {} //讓子類 override

    // --- 水球控制 ---
    virtual int getMaxWaterBombs() const { return maxWaterBombs; }
    virtual int getCurrentWaterBombs() const { return currentWaterBombs; }

    virtual void setMaxWaterBombs(int m) { maxWaterBombs = m; }
    virtual void setCurrentWaterBombs(int c) { currentWaterBombs = c; }

    virtual void increaseCurrentWaterBombs() { ++currentWaterBombs; }
    virtual void decreaseCurrentWaterBombs() { if (currentWaterBombs > 0) --currentWaterBombs; }

    virtual bool canPlaceBomb() const { return currentWaterBombs < maxWaterBombs; }

    // --- 貼圖取得 ---
    virtual QString getFrameKey() const = 0;  // 每個角色實作自己的 key
    void nextFrame(int mod) {
        frameIndex++;
        if (frameIndex > mod) frameIndex = 1;
    }
    void resetFrame() { frameIndex = 1; }
    int getFrameIndex() const { return frameIndex; }

    // --- 碰撞區 ---
    virtual QRect getCollisionBox() const {
            // 預設回傳整張圖大小作為碰撞框
            QPixmap sprite = SpriteSheetManager::instance().getFrame(getFrameKey());
            int displayWidth = 50;
            int displayHeight = sprite.height() * displayWidth / sprite.width();
            QPointF pos = getScreenPos();
            return QRect(pos.x(), pos.y(), displayWidth, displayHeight);
        }


protected:
    QPoint gridPos = {-1, -1};
    QPointF screenPos = {0, 0};
    Direction direction = Direction::Down;

    int hp = 1;
    int maxWaterBombs = 0;
    int currentWaterBombs = 0;

    int frameIndex = 1;
};
