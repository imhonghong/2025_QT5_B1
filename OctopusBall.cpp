#include "OctopusBall.h"
#include "SpriteSheetManager.h"
#include "BattleScene.h"

#include "Explosion.h"
#include <QDebug>

OctopusBall::OctopusBall(QPoint start, Direction dir, BattleScene* scene)
    : QObject(nullptr), gridPos(start), direction(dir), scene(scene) {
    timer.start();
    moveTimer.start();
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

    if (moveTimer.elapsed() >= 400) {  // ✅ 每 0.4 秒移動一次
        moveAndCheck();
        moveTimer.restart();
    }

    updateFrame();  // 更新動畫等
}

void OctopusBall::moveAndCheck() {
    if (!scene) return;

    QPoint nextPos = gridPos;
    switch (direction) {
        case Direction::Up:    nextPos.ry() -= 1; break;
        case Direction::Down:  nextPos.ry() += 1; break;
        case Direction::Left:  nextPos.rx() -= 1; break;
        case Direction::Right: nextPos.rx() += 1; break;
    }

    if (!scene->isInsideMap(nextPos)) {
        reverseDirection();
        return;
    }

    int tile = scene->getMap(nextPos);
    if (tile == 1 || tile == 2 || tile == 3) {  // 被磚/牆阻擋
        reverseDirection();
        return;
    }
    // ✅ 章魚碰撞反彈
    Octopus* octo = scene->getOctopus();
    if (!octo) return;
    if (octo->getCollisionBox().intersects(this->getBoundingBox())) {
        qDebug() << "[OctopusBall] 碰到章魚，反彈";
        reverseDirection();
    }
    gridPos = nextPos;
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

void OctopusBall::reverseDirection() {
    switch (direction) {
        case Direction::Up:    direction = Direction::Down; break;
        case Direction::Down:  direction = Direction::Up; break;
        case Direction::Left:  direction = Direction::Right; break;
        case Direction::Right: direction = Direction::Left; break;
    }
}
