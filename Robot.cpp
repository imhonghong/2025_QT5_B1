#include "Robot.h"
#include "PathFinder.h"
#include "WaterBomb.h"
#include "BattleScene.h"
#include "HashUtil.h"

#include <QDebug>
#include <QVector>
#include <QSet>

Robot::Robot() {}

void Robot::generatePlan(const QVector<QVector<int>>& map, const QPoint& playerPos) {
    plan.clear();
    stepIndex = 0;

    PathFinder pf(map);
    PathResult res = pf.findPath(getGridPos(), playerPos);

    if (res.path.isEmpty()) {
        qDebug() << "[Robot] 找不到通往 player 的路徑";
        return;
    }

    const QVector<QPoint>& path = res.path;
    QSet<QPoint> bombSet;
    for (const QPoint& p : res.bombSpots) {
        bombSet.insert(p);
    }

    qDebug() << "[Robot] path:" << path;
    qDebug() << "[Robot] bombSpots:" << res.bombSpots;

    for (int i = 1; i < path.size(); ++i) {
        const QPoint& current = path[i];

        // 一般移動
        plan.push_back({ RobotAction::MoveTo, current });

        // 若是炸彈點，插入炸退腳本
        if (bombSet.contains(current)) {
            // 放炸彈
            plan.push_back({ RobotAction::PlaceBomb, current });

            // 回退（逐格倒退兩步）
            int stepsBack = 0;
            for (int j = i - 1; j > 0 && stepsBack < 2; --j) {
                plan.push_back({ RobotAction::MoveTo, path[j] });
                stepsBack++;
            }

            // 找不到兩步就用 fallback
            if (stepsBack < 2) {
                QPoint alt = findEscapePointAround(current, map);
                if (alt != current)
                    plan.push_back({ RobotAction::MoveTo, alt });
            }

            // 等待
            plan.push_back({ RobotAction::Wait, getGridPos(), 190 });
        }
    }
}


bool Robot::isSafeToRetreat(const QPoint& bombPoint, const QVector<QPoint>& path, const QVector<QVector<int>>& map) const {
    int idx = path.indexOf(bombPoint);
    if (idx < 2) return false;

    QPoint escape = path[idx - 2];
    if (escape.y() < 0 || escape.y() >= map.size()) return false;
    if (escape.x() < 0 || escape.x() >= map[0].size()) return false;

    return map[escape.y()][escape.x()] == 0;
}

QPoint Robot::findEscapePointAround(const QPoint& bombPoint, const QVector<QVector<int>>& map) const {
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dy = -2; dy <= 2; ++dy) {
            if (abs(dx) + abs(dy) != 2) continue;
            QPoint p = bombPoint + QPoint(dx, dy);
            if (p.x() < 0 || p.x() >= map[0].size() || p.y() < 0 || p.y() >= map.size()) continue;
            if (map[p.y()][p.x()] == 0) return p;
        }
    }
    return bombPoint;  // fallback: 沒得逃就站原地（雖然會被炸死）
}

void Robot::advanceStep() {
    if (stepIndex >= plan.size()) return;

    Step& current = plan[stepIndex];

    switch (current.action) {
        case RobotAction::MoveTo: {
            QPoint delta = current.pos - getGridPos();
            if (delta == QPoint(0, 1)) setDirection(Direction::Down);
            else if (delta == QPoint(0, -1)) setDirection(Direction::Up);
            else if (delta == QPoint(-1, 0)) setDirection(Direction::Left);
            else if (delta == QPoint(1, 0)) setDirection(Direction::Right);

            setGridPos(current.pos);
            qDebug() << "Move to: " << current.pos;
            isMoving = true;
            nextFrame(4);
            break;
        }
        case RobotAction::PlaceBomb: {
            if (scene) {
                WaterBomb* bomb = new WaterBomb(current.pos);
                scene->addWaterBomb(bomb);
                qDebug() << "Place Bomb at: " << current.pos;
                isMoving = false;
            }
            break;
        }
        case RobotAction::Wait: {
            if (current.wait > 1) {
                current.wait--;
                qDebug() << "Wait at: " << current.pos;
                isMoving = false;
                return;
            }
            break;
        }
    } //switch end
    stepIndex++;
}


bool Robot::isFinished() const {
    return stepIndex >= plan.size();
}

void Robot::reset() {
    plan.clear();
    stepIndex = 0;
}


QString Robot::getFrameKey() const {
    QString dirStr;
    switch (getDirection()) {
        case Direction::Down: dirStr = "down"; break;
        case Direction::Up: dirStr = "up"; break;
        case Direction::Left: dirStr = "left"; break;
        case Direction::Right: dirStr = "right"; break;
    }

    if (isMoving)
        return QString("R_walk_%1_%2").arg(dirStr).arg(getFrameIndex());
    else
        return QString("R_stand_%1_1").arg(dirStr);
}

QPixmap Robot::getCurrentPixmap() const {
    QString frameKey = getFrameKey();
    return SpriteSheetManager::instance().getFrame(frameKey);
}

void Robot::generateTestPlan(){
    qDebug() << "[Robot] generateTestPlan 被呼叫！";
    QPoint p0 = getGridPos();
    QPoint p1 = p0 + QPoint(1, 0);  // 右邊
    QPoint p2 = p1 + QPoint(1, 0);
    QPoint p3 = p2 + QPoint(1, 0);
    plan = {
        { RobotAction::MoveTo, p1},
        { RobotAction::Wait, p1, 1},
        { RobotAction::MoveTo, p2},
        { RobotAction::PlaceBomb, p2 },
        { RobotAction::MoveTo, p3 },
        { RobotAction::Wait, p3, 30},
        { RobotAction::PlaceBomb, p3 },
        { RobotAction::MoveTo, p3+QPoint(1, 0) },
        { RobotAction::Wait, p3+QPoint(1, 0), 3 }
    };
}

void Robot::onDie() {
    qDebug() << "[Robot] onDie 被呼叫，觸發結束";
    emit requestEndGame(false);
}
