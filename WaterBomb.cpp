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


QPoint WaterBomb::getGridPos() const {
    return gridPos;
}

bool WaterBomb::getHasExploded() const {
    return hasExploded;
}

void WaterBomb::explode() {
    if (!hasExploded) {
        hasExploded = true;
        emit exploded(gridPos);
        qDebug() << "[WaterBomb] 被連鎖引爆 at" << gridPos;
    }
}

void WaterBomb::tick() {
    if (hasExploded) return;

    qint64 elapsed = timer.elapsed();

    if (elapsed >= 3000) {
        hasExploded = true;
        emit exploded(gridPos);
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
