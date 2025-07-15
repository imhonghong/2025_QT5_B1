#include "Robot.h"
#include "PathFinder.h"
#include "WaterBomb.h"
#include "BattleScene.h"

#include <QDebug>
#include <QVector>


Robot::Robot() {}

void Robot::generatePlan(const QVector<QVector<int>>& map, const QPoint& playerPos) {
    plan.clear();
    stepIndex = 0;

    PathFinder pf(map);
    QVector<QPoint> path = pf.findPath(getGridPos(), playerPos);

    if (path.isEmpty()) {
        qDebug() << "[Robot] 找不到通往 player 的路徑";
        return;
    }
    qDebug() << "[Robot] path length: " << path.size();
    qDebug() << "[Robot] 計算路徑:" << path;



    for (int i = 1; i < path.size(); ++i) {
        plan.push_back({ RobotAction::MoveTo, path[i] });
    }

    // 模擬放水球 + 倒退一步 + 等待引爆
    plan.push_back({ RobotAction::PlaceBomb, path.last() });
    if (path.size() >= 2)
        plan.push_back({ RobotAction::MoveTo, path[path.size() - 2] });
    plan.push_back({ RobotAction::Wait, path[path.size() - 2], 3 });
}

void Robot::advanceStep() {

    if (stepIndex >= plan.size()) return;
    qDebug() << "[Robot] stepIndex =" << stepIndex << "計畫總長:" << plan.size();

    Step current = plan[stepIndex];

    switch (current.action) {
    case RobotAction::MoveTo:
        setGridPos(current.pos);
        break;
    case RobotAction::PlaceBomb:
        qDebug() << "[Robot] 放水球於" << current.pos;
        if (scene) {
            WaterBomb* bomb = new WaterBomb(current.pos);
            scene->addWaterBomb(bomb);
        }
        break;
    case RobotAction::Wait:
        if (current.wait > 1) {
            plan[stepIndex].wait--;
            return;
        }
        break;
    }

    stepIndex++;
}


bool Robot::isFinished() const {
    return stepIndex >= plan.size();
}

void Robot::reset() {
    plan.clear();
    stepIndex = 0;
}


QString Robot::getCurrentSprite() const {
    QString dirStr;
    switch (direction) {
    case 0: dirStr = "down"; break;
    case 1: dirStr = "up"; break;
    case 2: dirStr = "left"; break;
    case 3: dirStr = "right"; break;
    }

    if (isMoving)
        return QString("R_walk_%1_%2").arg(dirStr).arg(frameIndex + 1);
    else
        return QString("R_stand_%1_1").arg(dirStr);
}

QPixmap Robot::getCurrentPixmap() const {
    QString frameKey = getCurrentSprite();
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
