#pragma once

#include <QWidget>
#include <QVector>
#include <QTimer>

#include "Player.h"
#include "Robot.h"
#include "GameMode.h"
#include "Monster.h"
#include "Octopus.h"
#include "IGameController.h"
#include "WaterBomb.h"
#include "Explosion.h"

class BattleScene : public QWidget {
    Q_OBJECT
public:
    explicit BattleScene(QWidget *parent = nullptr);

    int getMap(const QPoint& p) const; //單格地圖
    QVector<QVector<int>> getCurrentMap() const; //尋路用，整張地圖
    void setMap(const QPoint& p, int val); //for explosion
    void setMap(const QVector<QVector<int>>& map); //for read txt

    void setPlayer(Player* player);
    void setRobot(Robot* r);
    void setController(IGameController* c);

    QPoint getPlayerGridPos() const; //尋路用

    Player* getPlayer() const;
    Robot* getRobot() const;
    const QVector<Monster*>& getMonsters() const;
    const QVector<Octopus*>& getOctopi() const;

    void addMonster(Monster* monster);
    void addWaterBomb(WaterBomb* bomb);

    const QVector<WaterBomb*>& getWaterBombs() const;

    //mode1
    void incrementStepCount() { ++stepCount; update(); }


protected:
    void paintEvent(QPaintEvent* event) override;

signals:
    void backToTitle();
    void gameEnded(bool isWin);

private:
    QVector<QVector<int>> mapData;

    // characters
    Player* player = nullptr;
    Robot* robot = nullptr;
    IGameController* controller = nullptr;
    QVector<Monster*> monsters;
    QVector<Octopus*> octopi;

    // bombs
    QVector<WaterBomb*> waterBombs;
    QVector<Explosion*> explosions;

    QTimer updateTimer;

    //for mode1
    int stepCount = -1;

};
