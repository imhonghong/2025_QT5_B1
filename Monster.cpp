#include "Monster.h"
#include "SpriteSheetManager.h"
#include <cmath>

Monster::Monster(const QPoint& pos) {
    setGridPos(pos);
    setDirection(Direction::Down);  // 初始朝下
    setHp(1);  // 固定血量
}

Monster::Monster(const QPoint& pos, const QRect& roam, bool cw)
    : roamZone(roam), clockwise(cw), pattern(cw ? "clockwise" : "counterclockwise")
{
    setGridPos(pos);
    setDirection(Direction::Down);
    setHp(1);
    initStartDirection(pos);
    // 可選擇存 roamZone，若你未來要用
}

QString Monster::getFrameKey() const {
    QString dir;
    switch (getDirection()) {
        case Direction::Down:  dir = "down"; break;
        case Direction::Left:  dir = "left"; break;
        case Direction::Right: dir = "right"; break;
        case Direction::Up:    dir = "up"; break;
    }
    return QString("M_walk_%1_%2").arg(dir).arg(getFrameIndex());
}

Direction Monster::nextDirection(Direction dir, bool cw) const {
    // 順時針轉向順序：Up → Right → Down → Left
    // 逆時針轉向順序：Up → Left → Down → Right
    static const QVector<Direction> clockwiseSeq   = {Direction::Up, Direction::Right, Direction::Down, Direction::Left};
    static const QVector<Direction> counterSeq     = {Direction::Up, Direction::Left, Direction::Down, Direction::Right};

    const auto& seq = cw ? clockwiseSeq : counterSeq;
    int idx = seq.indexOf(dir);
    return seq[(idx + 1) % 4];
}

bool Monster::isOnEdge(const QPoint& p) const {
    int left = roamZone.left();
    int right = roamZone.right();
    int top = roamZone.top();
    int bottom = roamZone.bottom();

    return (
        (p.x() >= left && p.x() <= right && (p.y() == top || p.y() == bottom)) ||
        (p.y() >= top && p.y() <= bottom && (p.x() == left || p.x() == right))
    );
}

void Monster::updateMovement() {
    if (!isMoving) {
        // 計算下一個 grid 目標
        QPoint currGrid = getGridPos();
        QPoint next;

        switch (currentDir) {
            case Direction::Up:    next = currGrid + QPoint(0, -1); break;
            case Direction::Down:  next = currGrid + QPoint(0, 1);  break;
            case Direction::Left:  next = currGrid + QPoint(-1, 0); break;
            case Direction::Right: next = currGrid + QPoint(1, 0);  break;
        }

        if (isOnEdge(next)) {
            gridTarget = next;
            isMoving = true;
        } else {
            currentDir = nextDirection(currentDir, clockwise);
            setDirection(currentDir);
        }
    }

    // 如果正在移動
    if (isMoving) {
        QPointF curr = getScreenPos();
        QPointF dest = QPointF(gridTarget.x() * 50, gridTarget.y() * 50);

        // 向目標前進
        QPointF delta = dest - curr;
        float dist = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

        if (dist < moveSpeed * 50) {
            // 到達目標
            setScreenPos(dest);
            setGridPos(gridTarget);
            isMoving = false;
        } else {
            // 單位方向 * speed
            QPointF unit = delta / dist;
            setScreenPos(curr + unit * (moveSpeed * 50));
            frameTimer += moveSpeed;  // ✅ moveSpeed 代表移動速度與時間成正比

            if (frameTimer >= frameInterval) {
                nextFrame(2);
                frameTimer = 0.0f;
            }
        }

        // 設定面向
        setDirection(currentDir);
    }
}

void Monster::initStartDirection(const QPoint& pos) {
    if (pos.y() == roamZone.top() && pos.x() < roamZone.right())
        currentDir = clockwise ? Direction::Right : Direction::Left;
    else if (pos.x() == roamZone.right() && pos.y() < roamZone.bottom())
        currentDir = clockwise ? Direction::Down : Direction::Up;
    else if (pos.y() == roamZone.bottom() && pos.x() > roamZone.left())
        currentDir = clockwise ? Direction::Left : Direction::Right;
    else if (pos.x() == roamZone.left() && pos.y() > roamZone.top())
        currentDir = clockwise ? Direction::Up : Direction::Down;
    else
        currentDir = Direction::Right;

    setDirection(currentDir);
}
