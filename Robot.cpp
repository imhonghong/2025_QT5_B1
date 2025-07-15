#include "Robot.h"
#include "PathFinder.h"
#include "WaterBomb.h"
#include "BattleScene.h"
#include "HashUtil.h"

#include <QDebug>
#include <QVector>
#include <QSet>
#include <QHash>


Robot::Robot() {}

void Robot::generateOneStepPlan(const QVector<QVector<int>>& map, const QPoint& playerPos) {
    plan.clear();
    stepIndex = 0;

    PathFinder pf(map);
    QVector<QPoint> path;
    QPoint bombPos;

    // 嘗試找炸牆點
    if (pf.findNextBombPlan(getGridPos(), bombPos, path)) {
        QPoint escape;
        if (!canEscapeFrom(bombPos, map, escape)) {
            qDebug() << "[Robot] 找到炸點但無法逃離，放棄此次炸彈";
            return;
        }

        // 加入移動步驟
        for (int i = 1; i < path.size(); ++i) {
            plan.push_back({ RobotAction::MoveTo, path[i] });
        }

        // 放炸 + 躲避 + 等待
        plan.push_back({ RobotAction::PlaceBomb, bombPos });
        plan.push_back({ RobotAction::MoveTo, escape });
        plan.push_back({ RobotAction::Wait, escape, 6 });
        return;
    }

    // 找不到炸牆點，嘗試炸 player
    static const QVector<QPoint> aroundPlayer = {
        playerPos + QPoint(1, 0), playerPos + QPoint(-1, 0),
        playerPos + QPoint(0, 1), playerPos + QPoint(0, -1)
    };

    for (const QPoint& pos : aroundPlayer) {
        if (map[pos.y()][pos.x()] != 0) continue;

        QVector<QPoint> path = pf.findPath(getGridPos(), pos);
        if (path.isEmpty()) continue;

        QPoint escape;
        if (!canEscapeFrom(pos, map, escape)) continue;

        for (int i = 1; i < path.size(); ++i) {
            plan.push_back({ RobotAction::MoveTo, path[i] });
        }

        plan.push_back({ RobotAction::PlaceBomb, pos });
        plan.push_back({ RobotAction::MoveTo, escape });
        plan.push_back({ RobotAction::Wait, escape, 6 });
        return;
    }

    qDebug() << "[Robot] 找不到可行策略";
}




bool Robot::canEscapeFrom(const QPoint& bombPoint, const QVector<QVector<int>>& map, QPoint& escapePoint) const {
    const int rows = map.size();
    const int cols = map[0].size();

    for (int dx = -2; dx <= 2; ++dx) {
        for (int dy = -2; dy <= 2; ++dy) {
            if (abs(dx) + abs(dy) != 2) continue; // 只取曼哈頓距離為2的格子

            QPoint candidate = bombPoint + QPoint(dx, dy);
            if (candidate.x() < 0 || candidate.x() >= cols || candidate.y() < 0 || candidate.y() >= rows)
                continue;

            if (map[candidate.y()][candidate.x()] == 0) {
                escapePoint = candidate;
                return true;
            }
        }
    }

    return false;
}



void Robot::startAutoAdvance() {
    if (!stepTimer) {
        stepTimer = new QTimer(this);
        connect(stepTimer, &QTimer::timeout, this, [this]() {
            this->advanceStep();
        });
        stepTimer->start(500); // 每秒執行一次
        qDebug() << "[Robot] 自動移動計時器啟動";
    }
}

void Robot::advanceStep() {
    if (stepIndex >= plan.size()) {
        if (scene) {
            QVector<QVector<int>> updatedMap = scene->getCurrentMap();
            QPoint playerPos = scene->getPlayerGridPos();
            generateOneStepPlan(updatedMap, playerPos);
        }
        return;
    }

    Step& current = plan[stepIndex];

    switch (current.action) {
        case RobotAction::MoveTo: {
            QPoint delta = current.pos - getGridPos();
            if (delta == QPoint(0, 1)) setDirection(Direction::Down);
            else if (delta == QPoint(0, -1)) setDirection(Direction::Up);
            else if (delta == QPoint(-1, 0)) setDirection(Direction::Left);
            else if (delta == QPoint(1, 0)) setDirection(Direction::Right);

            setGridPos(current.pos);
            isMoving = true;
            nextFrame(4);
            break;
        }
    case RobotAction::PlaceBomb: {
            if (scene) {
                WaterBomb* bomb = new WaterBomb(current.pos);
                scene->addWaterBomb(bomb);
                isMoving = false;
            }
            break;
        }
    case RobotAction::Wait: {
            if (current.wait > 1) {
                current.wait--;
                isMoving = false;
                return;
            }
            break;  // wait 結束
        }
    }

    stepIndex++;  // ☑️ 統一推進 stepIndex
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

