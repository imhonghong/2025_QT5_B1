#pragma once

#include <QObject>
#include "Character.h"
#include <QSet>
#include <QTimer>
#include <QHash>


class BattleScene;
class IGameController;

enum class ItemType {
    Needle,
    Turtle,
    SpeedShoes,
    ExtraBomb,
    Glove,
    MoonWalk
};

enum class PlayerState {
    Standing,
    Walking,
    TurtleStanding,
    TurtleWalking,
    Trapped,
    Dead
};

inline uint qHash(ItemType key, uint seed = 0) {
    return ::qHash(static_cast<int>(key), seed);
}
inline uint qHash(Direction key, uint seed = 0) {
    return ::qHash(static_cast<int>(key), seed);
}

class Player : public QObject, public Character {
    Q_OBJECT
public:
    Player();

    void setController(IGameController* controller);
    void update(float delta);
    void setScene(BattleScene* s);

    void enterTrappedState();
    int getNeedleCount() const { return needleCount; }
    void tryRescue();               // 使用針戳破水球
    void tryPlaceWaterBomb();   //放水球
    void onTrappedTimeout();       // 倒數三秒後觸發
    bool getIsTrapped() const { return state == PlayerState::Trapped; }

    void addItem(ItemType item);
    bool hasItem(ItemType item) const;

    void takeDamage(int dmg = 1) override;
    void onDie() override;
    QString getFrameKey() const override;
    QRect getCollisionBox() const override;

    void addMoveKey(Direction dir);
    void removeMoveKey(Direction dir);


signals:
    void requestEndGame(bool isWin);

private:
    IGameController* controller = nullptr;
    BattleScene* scene = nullptr;

    int maxHp = 3;
    int needleCount = 0;
    int extraBombCount = 0;

    // 漂浮倒數計時
    QTimer* trappedTimer = nullptr;
    QSet<ItemType> itemSet;

    PlayerState state = PlayerState::Standing;

    Direction currentMoveDir = Direction::Down; // 現在正在移動方向
    int moveSpeed = 60;                         // 預設速度
    QSet<Direction> activeKeys;                 // 正在按的方向鍵

    QPoint getNearestGridPos() const; //放水球整數點
};


