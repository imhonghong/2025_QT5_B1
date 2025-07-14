#pragma once
#include "Character.h"
#include "SpriteSheetManager.h"

#include <QVector>
#include <QPoint>

class BattleScene;
class IGameController;
enum class RobotAction {
    MoveTo,
    PlaceBomb,
    Wait
};

struct Step {
    RobotAction action;
    QPoint pos;
    int wait;

    Step() : action(RobotAction::Wait), pos(0, 0), wait(0) {} // 預設建構子
    Step(RobotAction a, const QPoint& p, int w = 0)
        : action(a), pos(p), wait(w) {}
};


class Robot : public Character {
public:
    Robot();
    void setScene(BattleScene* s) { scene = s; }

    void generatePlan(const QVector<QVector<int>>& map, const QPoint& playerPos);
    void advanceStep();
    void generateTestPlan(); //only for testing

    bool isFinished() const;
    void reset();

    QString getCurrentSprite() const;
    QPixmap getCurrentPixmap() const;


    void setStepCount(int step) { stepCount = step; }
    int getStepCount() const { return stepCount; }

private:
    QVector<Step> plan;
    BattleScene* scene = nullptr;

    int stepIndex = 0;
    int direction = 0;     // 0=down, 1=up, 2=left, 3=right
    int frameIndex = 0;
    bool isMoving = false;
    int stepCount = 0;
};
