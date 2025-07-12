#pragma once

#include <QWidget>
#include <QVector>

#include "Player.h"
#include "GameMode.h"
#include "Robot.h"
#include "Monster.h"

class BattleScene : public QWidget {
    Q_OBJECT
public:
    explicit BattleScene(QWidget *parent = nullptr);

    void setMap(const QVector<QVector<int>>& map);
    void setPlayer(Player* player);
    void setup(GameMode mode);

    void addRobot(Robot* robot);
    void addMonster(Monster* monster);


protected:
    void paintEvent(QPaintEvent* event) override;

signals:
    void backToTitle();
    void gameEnded(bool isWin);

private:
    QVector<QVector<int>> mapData;
    Player* player = nullptr;
    GameController* controller = nullptr;
    QVector<Robot*> robots;
    QVector<Monster*> monsters;
};
