#include "WaterBomb.h"
#include "BattleScene.h"
#include "SpriteSheetManager.h"
#include <QDebug>

WaterBomb::WaterBomb(QPoint pos, QObject* parent)
    : QObject(parent), gridPos(pos) {
    timer.start();
}

WaterBomb::WaterBomb(QPoint gridPos, BattleScene* scene, Player* owner)
    : QObject(scene), gridPos(gridPos), owner(owner) {
    timer.start();
}

WaterBomb::WaterBomb(QPoint gridPos, BattleScene* scene, Player* owner, int range)
    : QObject(scene), gridPos(gridPos), owner(owner), range(range) {
    timer.start();
}


QPoint WaterBomb::getGridPos() const {
    return gridPos;
}

bool WaterBomb::getHasExploded() const {
    return hasExploded;
}

void WaterBomb::explode() {
    if (!hasExploded) {
        qDebug() << "[WB] if (!hasexploded)";
        hasExploded = true;
        emit exploded(gridPos);
        if (owner) {
            qDebug() << "[WaterBomb] 有 owner，開始呼叫 decreaseCurrentWaterBombs()";
            owner->decreaseCurrentWaterBombs();
        } else {
            qDebug() << "[WaterBomb] 沒有 owner，無法通知玩家";
        }
    }
}
void WaterBomb::tick() {
    if (hasExploded) return;

    qint64 elapsed = timer.elapsed();

    if (elapsed >= 3000) {
        explode();
        qDebug() << "[WaterBomb] 💥爆炸於" << gridPos;
    } else {
        currentFrame = 1 + (elapsed / 800) % 4; // 換動畫 WB_1~4
    }
}

QPixmap WaterBomb::getCurrentPixmap() const {
    if (hasExploded) {
         return QPixmap();
    }
    return SpriteSheetManager::instance().getFrame(QString("WB_%1").arg(currentFrame));
}
