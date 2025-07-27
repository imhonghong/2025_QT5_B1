#pragma once

#include <QObject>
#include <QSet>
#include <QTimer>
#include <QHash>

#include "Item.h"
#include "Character.h"

class BattleScene;
class IGameController;



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
    QPoint getStartPos(int waveIndex);

    void enterTrappedState();
    int getNeedleCount() const { return needleCount; }
    void tryRescue();               // 使用針戳破水球

    void tryPlaceWaterBomb();       //放水球
    void onTrappedTimeout();        // 倒數三秒後觸發
    void onTurtleBreak();
    bool getIsTrapped() const { return state == PlayerState::Trapped; }
    void setStateStanding();

    void addItem(ItemType item);
    bool hasItem(ItemType item) const;
    void removeItem(ItemType item) { itemSet.remove(item); }

    void takeDamage(int dmg = 1) override;
    void onDie() override;
    void updateMoveSpeed();

    QString getFrameKey() const override;
    QRect getCollisionBox() const override;

    void addMoveKey(Direction dir);     //移動
    void removeMoveKey(Direction dir);  //移動

    bool isInvincible() const { return invincible; }

    void setGridAlignedScreenPos(QPoint gridPos); // 新增宣告


signals:
    void requestEndGame(bool isWin);

private:
    IGameController* controller = nullptr;
    BattleScene* scene = nullptr;
    QPoint startGridPos;

    int maxHp = 3;
    int needleCount = 0;
    int powerPotionCount = 0;

    // 漂浮倒數計時
    QTimer* trappedTimer = nullptr;
    QSet<ItemType> itemSet;
    int trappedFrameIndex = 0;
    QTimer* trappedAnimTimer = nullptr;
    QTimer* recoverTimer = nullptr; // 浮完泡泡再復活
    int recoverFrameCount = 0;      // P_wd_1/2 frame index

    PlayerState state = PlayerState::Standing;
    Direction currentMoveDir = Direction::Down; // 現在正在移動方向
    int moveSpeed = 75;                         // 預設速度
    QSet<Direction> activeKeys;                 // 正在按的方向鍵

    QPoint getNearestGridPos() const; //放水球整數點

    QTimer* invincibleTimer = nullptr;
    bool invincible = false;
    int invincibleFrameCounter = 0;  // private 區



};


