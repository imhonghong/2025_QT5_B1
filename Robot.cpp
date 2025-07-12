#include "Robot.h"
#include <QDebug>


Robot::Robot() {
    position = QPoint(-1, -1);
}

void Robot::generatePlan(const QVector<QVector<int>>& map, const QPoint& playerPos) {
    plan.clear();
    stepIndex = 0;

    // 這裡暫時直接走向 player（未考慮磚塊或放炸彈）
    // 實作 BFS 可加入判斷 type==3 時加權重，但這裡先手動建路徑（示意）
    QPoint cur = position;
    while (cur != playerPos) {
        QPoint next = cur;
        if (cur.x() < playerPos.x()) next.setX(cur.x() + 1);
        else if (cur.x() > playerPos.x()) next.setX(cur.x() - 1);
        else if (cur.y() < playerPos.y()) next.setY(cur.y() + 1);
        else if (cur.y() > playerPos.y()) next.setY(cur.y() - 1);

        plan.push_back({ RobotAction::MoveTo, next });
        cur = next;
    }

    // 模擬抵達後放炸彈 + 退後 + 等待引爆
    plan.push_back({ RobotAction::PlaceBomb, cur });
    if (plan.size() >= 2)
        plan.push_back({ RobotAction::MoveTo, plan[plan.size() - 2].pos }); // 倒退一步
    plan.push_back({ RobotAction::Wait, cur, 3 }); // 等待3回合
}

void Robot::advanceStep() {
    if (stepIndex >= plan.size()) return;

    Step current = plan[stepIndex];

    switch (current.action) {
    case RobotAction::MoveTo:
        setGridPos(current.pos);
        break;
    case RobotAction::PlaceBomb:
        qDebug() << "[Robot] Placed bomb at" << current.pos;
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

