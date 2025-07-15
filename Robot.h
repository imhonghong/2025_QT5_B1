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


class Robot : public QObject, public Character {
    Q_OBJECT
public:
    Robot();
    void setScene(BattleScene* s) { scene = s; }

    void generatePlan(const QVector<QVector<int>>& map, const QPoint& playerPos);
    void advanceStep();
    void generateTestPlan(); //only for testing

    bool isFinished() const;
    void reset();

    QString getFrameKey() const override;
    void onDie() override;
    QPixmap getCurrentPixmap() const;


    void setStepCount(int step) { stepCount = step; }
    int getStepCount() const { return stepCount; }

signals:
    void requestEndGame(bool isWin = false);  // mode1 中 robot 死亡代表輸

private:
    QVector<Step> plan;
    BattleScene* scene = nullptr;
    bool isMoving = false;
    int stepIndex = 0;
    int stepCount = 0;
};
