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
#include "Item.h"
#include "OctopusBall.h"

class BattleScene : public QWidget {
    Q_OBJECT
public:
    explicit BattleScene(QWidget *parent = nullptr);
    // mode1
    void setMap(const QVector<QVector<int>>& map); //for read txt
    void incrementStepCount() { ++stepCount; update(); }
    Robot* getRobot() const;
    void setRobot(Robot* r);
    void addWaterBomb(WaterBomb* bomb); //for robot
    void setPlayer(Player* p) {
        player = p;
        if (player) player->setScene(this);
    };

    // mode2
    QTimer updateTimer;
    Player* getPlayer() const {
        return controller ? controller->getPlayer() : nullptr;
    }
    Octopus* getOctopus() const { return octopus; }
    void addMonster(Monster* monster);
    void addPlayer(Player* p, const QPoint& pos);
    void addBrick(const QPoint& pos, int type);
    void addOctopus(Octopus* o);
    void removeItem(QObject* item);
    void clearScene();
    void clearItems(); // 清除場上的所有 Item
    bool checkCollision(const QRect& box) const;
    bool checkCollisionExcludingMonsters(const QRect& box) const;

    // void addWaterBomb(QPoint gridPos, Player* owner);  // for Player
    void addWaterBomb(QPoint gridPos, Player* owner, int range, bool isGlove);
    bool hasWaterBomb(const QPoint& gridPos) const;
    int getWaterBombCount(Player* owner) const;

    // ----- for octopus ----- //
    void addOctopusBall(OctopusBall* ball);
    void onOctopusBallExplode(OctopusBall* ball);
    void removeOctopusBall(OctopusBall* ball);
    bool isInsideMap(QPoint pos) const;
    QPoint getOctopusGridPos() const {
        return octopus ? octopus->getGridPos() : QPoint(-1, -1);
    }

    void updateOctoBall();
    // ----- for octopus ----- //

    void addItem(Item* item);  // 新增
    QVector<Item*>& getItems(); // 讓 player 檢查

    // ----- 可推磚 ----- //
    bool tryPushBrick(const QRect& playerBox, Direction dir);
    // ----- 可推磚 ----- //

    // mode1+2
    GameMode getMode() const { return mode; }
    void setMode(GameMode m){ mode = m; }

    int getMap(const QPoint& p) const; //單格地圖
    QVector<QVector<int>> getCurrentMap() const; //尋路用，整張地圖
    void setMap(const QPoint& p, int val); //for explosion
    void bindPlayerForMode1(Player* p);
    void setController(IGameController* c);
    QPoint getPlayerGridPos() const; //尋路用

    const QVector<Monster*>& getMonsters() const;
    const QVector<Octopus*>& getOctopi() const;
    const QVector<WaterBomb*>& getWaterBombs() const;
    void togglePause();
    void handleGameEnd(bool isWin);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

signals:
    void backToTitle();
    void gameEnded(bool isWin);
    void pauseRequested();
    void returnToMainMenu();
    void monsterRemoved(Monster* m);

private:
    QVector<QVector<int>> mapData;

    // characters
    Player* player = nullptr;
    Robot* robot = nullptr;
    IGameController* controller = nullptr;
    QVector<Monster*> monsters;
    Octopus* octopus = nullptr;
    QVector<Item*> items;

    // bombs
    QVector<WaterBomb*> waterBombs;
    QVector<Explosion*> explosions;
    QVector<OctopusBall*> octopusBalls;

    //for mode1
    int stepCount = -1;

    // painter
    void paintMap(QPainter& painter, SpriteSheetManager& sheet, int cellSize);
    void paintPlayer(QPainter& painter, SpriteSheetManager& sheet);
    void paintMonsters(QPainter& painter);
    void paintOctopus(QPainter& painter);
    void paintWaterBombs(QPainter& painter);
    void paintOctopusBall(QPainter& painter);
    void paintExplosions(QPainter& painter);
    void paintRobot(QPainter& painter);
    void paintItems(QPainter& painter);

    // UI
    void paintUI(QPainter& painter);
    QRect pauseButtonRect = QRect(200, 500, 37, 34);
    QRect homeButtonRect = QRect(240, 500, 34, 34);

    // pause
    bool isPaused = false;

    GameMode mode;

};
