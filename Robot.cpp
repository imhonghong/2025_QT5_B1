#include "Robot.h"
#include "PathFinder.h"
#include "WaterBomb.h"
#include "BattleScene.h"
#include "HashUtil.h"

#include <QDebug>
#include <QVector>
#include <QSet>

Robot::Robot() { }

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

    for (int i = 0; i < path.size(); ++i) {
        const QPoint& current = path[i];

        // 一般移動
        plan.push_back({ RobotAction::MoveTo, current });
        plan.push_back({ RobotAction::Wait, current, 30 }); //主動停頓

        // 若是炸彈點，插入炸退腳本
        if (bombSet.contains(current)) {
            // 放炸彈
            plan.push_back({ RobotAction::PlaceBomb, current });
            if ( i > 2){
                QVector<QPoint> retreat;

                retreat.push_back(path[i-1]);
                retreat.push_back(path[i-2]);
                plan.push_back({ RobotAction::MoveTo, path[i-1] });
                plan.push_back({ RobotAction::Wait, path[i-1], 30 });
                plan.push_back({ RobotAction::MoveTo, path[i-2] });
                plan.push_back({ RobotAction::Wait, path[i-2], 30 });
                plan.push_back({ RobotAction::Wait, getGridPos(), 190 }); // 3秒
                plan.push_back({ RobotAction::MoveTo, path[i-1] });
                plan.push_back({ RobotAction::Wait, path[i-1], 30 });
                plan.push_back({ RobotAction::MoveTo, path[i] });
                plan.push_back({ RobotAction::Wait, path[i], 30 });


            } else {
                EscapePlan eplan = generateEscapePlan(current, map);

                for (int j = 1; j < eplan.retreatPath.size(); ++j){
                    plan.push_back({ RobotAction::MoveTo, eplan.retreatPath[j] });
                    plan.push_back({ RobotAction::Wait, current, 30 }); //主動停頓
                }
                plan.push_back({ RobotAction::Wait, eplan.retreatPath.last(), 190 });  // 3 秒等待

                for (int j = 1; j < eplan.returnPath.size(); ++j){
                    plan.push_back({ RobotAction::MoveTo, eplan.returnPath[j] });
                    plan.push_back({ RobotAction::Wait, eplan.returnPath[j], 30 }); //主動停頓
                }
            }

        }
    }
}


void Robot::advanceStep() {
    if (stepIndex >= plan.size()) return;

    Step& current = plan[stepIndex];
    if (scene && current.action == RobotAction::MoveTo)
        scene->incrementStepCount();


    switch (current.action) {
        case RobotAction::MoveTo: {
            QPoint prevPos = getGridPos();              // ⬅️ 先記下原位置
            setGridPos(current.pos);                    // ⬅️ 然後移動
            QPoint delta = current.pos - prevPos;
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
                WaterBomb* bomb = new WaterBomb(current.pos, scene, nullptr, 1);
                scene->addWaterBomb(bomb);
                qDebug() << "Place Bomb at: " << current.pos;
                isMoving = false;
            }
            break;
        }
        case RobotAction::Wait: {
            if (current.wait > 1) {
                current.wait--;
                // qDebug() << "Wait at: " << current.pos;
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


void Robot::onDie() {
    qDebug() << "[Robot] onDie 被呼叫，觸發結束";
    emit requestEndGame(false);
}

EscapePlan Robot::generateEscapePlan(const QPoint& bombPoint, const QVector<QVector<int>>& map) {
    EscapePlan plan;

    QVector<QPoint> candidates = {
        bombPoint + QPoint(2,0), bombPoint + QPoint(-2,0),
        bombPoint + QPoint(0,2), bombPoint + QPoint(0,-2),
        bombPoint + QPoint(1,1), bombPoint + QPoint(-1,1),
        bombPoint + QPoint(1,-1), bombPoint + QPoint(-1,-1)
    };

    for (const QPoint& escape : candidates) {
        PathFinder pf(map);
        QVector<QPoint> toEscape = pf.bfs(bombPoint, escape);
        QVector<QPoint> toBack = pf.bfs(escape, bombPoint);

        if (!toEscape.isEmpty() && !toBack.isEmpty()) {
            plan.retreatPath = toEscape;
            plan.returnPath = toBack;
            return plan;
        }
    }

    return plan; // 如果沒找到，就回傳空的兩段 path
}
