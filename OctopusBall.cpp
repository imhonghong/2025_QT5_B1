#include "OctopusBall.h"
#include "SpriteSheetManager.h"

#include "Explosion.h"
#include <QDebug>

OctopusBall::OctopusBall(QPoint start, Direction dir, BattleScene* scene)
    : QObject(nullptr), gridPos(start), direction(dir), scene(scene) {
    timer.start();
}

OctopusBall::~OctopusBall() {
    // QDebug() << "OctopusBall destroyed";
}

void OctopusBall::tick() {
    if (hasExploded) return;

    if (isTimeUp()) {
        explode();
        return;
    }

    updateFrame();
}

void OctopusBall::updateFrame() {
    qint64 elapsed = timer.elapsed();
    currentFrame = 1 + (elapsed / 800) % 4;
}

void OctopusBall::explode() {
    if (hasExploded) return;
    hasExploded = true;

    emit requestExplosion(this);  // ★ 通知 scene 處理爆炸
    deleteLater();  // 自己清除
}

bool OctopusBall::isTimeUp() const {
    return timer.elapsed() >= 3000;
}

void OctopusBall::moveForward() {
    switch (direction) {
        case Direction::Up:    gridPos.ry() -= 1; break;
        case Direction::Down:  gridPos.ry() += 1; break;
        case Direction::Left:  gridPos.rx() -= 1; break;
        case Direction::Right: gridPos.rx() += 1; break;
    }
}

QPoint OctopusBall::getGridPos() const {
    return gridPos;
}

QRect OctopusBall::getBoundingBox() const {
    const int cellSize = 50;
    return QRect(gridPos.x() * cellSize, gridPos.y() * cellSize, cellSize, cellSize);
}

QPixmap OctopusBall::getCurrentPixmap() const {
    if (hasExploded) return QPixmap();
    return SpriteSheetManager::instance().getFrame(QString("WB_%1").arg(currentFrame));
}
