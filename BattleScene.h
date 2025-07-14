#pragma once

#include <QWidget>
#include <QVector>

#include "Player.h"
#include "GameMode.h"
#include "Monster.h"
#include "IGameController.h"
#include "WaterBomb.h"

class BattleScene : public QWidget {
    Q_OBJECT
public:
    explicit BattleScene(QWidget *parent = nullptr);

    void setMap(const QVector<QVector<int>>& map);
    void setPlayer(Player* player);
    void setController(IGameController* c);

    void addMonster(Monster* monster);
    void addWaterBomb(WaterBomb* bomb);


protected:
    void paintEvent(QPaintEvent* event) override;

signals:
    void backToTitle();
    void gameEnded(bool isWin);

private:
    QVector<QVector<int>> mapData;
    Player* player = nullptr;
    IGameController* controller = nullptr;
    QVector<Monster*> monsters;
    QVector<WaterBomb*> waterBombs;
};
