#pragma once

#include <QObject>
#include "Character.h"
#include <QSet>
#include <QTimer>
#include <QHash>


class IGameController;

enum class ItemType {
    Needle,
    Turtle,
    SpeedShoes,
    ExtraWB,
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

class Player : public QObject, public Character {
    Q_OBJECT
public:
    Player();

    void setController(IGameController* controller);
    void update(float delta);

    void enterTrappedState();
    void tryRescue();               // 使用針戳破水球
    void onTrappedTimeout();       // 倒數三秒後觸發
    bool getIsTrapped() const { return state == PlayerState::Trapped; }

    void addItem(ItemType item);
    bool hasItem(ItemType item) const;

    void takeDamage(int dmg = 1) override;
    void onDie() override;
    QString getFrameKey() const override;

signals:
    void requestEndGame(bool isWin);

private:
    IGameController* controller = nullptr;

    int maxHp = 3;

    // 漂浮倒數計時
    QTimer* trappedTimer = nullptr;
    QSet<ItemType> itemSet;

    PlayerState state = PlayerState::Standing;
};



