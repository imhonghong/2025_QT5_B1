#pragma once

#include <QWidget>
#include <QVector>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>

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
    // mode1
    void setMap(const QVector<QVector<int>>& map); //for read txt
    void incrementStepCount() { ++stepCount; update(); }
    Robot* getRobot() const;

    // mode2
    void addMonster(Monster* monster);
    void addPlayer(Player* p, const QPoint& pos);
    void addBrick(const QPoint& pos, int type);
    void removeItem(QObject* item);
    void clearScene();

    // mode1+2
    int getMap(const QPoint& p) const; //單格地圖
    QVector<QVector<int>> getCurrentMap() const; //尋路用，整張地圖
    void setMap(const QPoint& p, int val); //for explosion

    void setPlayer(Player* player);
    void setRobot(Robot* r);
    void setController(IGameController* c);

    QPoint getPlayerGridPos() const; //尋路用

    Player* getPlayer() const;

    const QVector<Monster*>& getMonsters() const;
    const QVector<Octopus*>& getOctopi() const;


    void addWaterBomb(WaterBomb* bomb);
    const QVector<WaterBomb*>& getWaterBombs() const;

    void togglePause();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void backToTitle();
    void gameEnded(bool isWin);
    void pauseRequested();
    void returnToMainMenu();

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

    // painter
    void paintMap(QPainter& painter, SpriteSheetManager& sheet, int cellSize);
    void paintPlayer(QPainter& painter, SpriteSheetManager& sheet);
    void paintMonsters(QPainter& painter);
    void paintWaterBombs(QPainter& painter);
    void paintExplosions(QPainter& painter);
    void paintRobot(QPainter& painter);

    // UI
    void paintUI(QPainter& painter);
    QRect pauseButtonRect = QRect(200, 500, 37, 34);
    QRect homeButtonRect = QRect(240, 500, 34, 34);

    // pause
    bool isPaused = false;

};
