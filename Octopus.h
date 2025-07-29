#pragma once
#include <QObject>
#include <QElapsedTimer>
#include "Character.h"
#include "Player.h"

class BattleScene;
class Octopus : public QObject, public Character {
    Q_OBJECT

public:
    Octopus();
    void hit(); // 被炸彈打中一次
    bool isDead() const override;
    void updateAI(); // 預留：依照玩家位置移動/攻擊
    void setScene(BattleScene* s) { scene = s; }

    void setPlayer(Player* p) { player = p; }
    QString getFrameKey() const override;
    QRect getCollisionBox() const override;
    void onDie() override;

private:
    BattleScene* scene = nullptr;
    int hp = 6;
    QElapsedTimer aiTimer;
    Player* player = nullptr; // ✅ 新增

    bool isDying = false;
    int dieFrameIndex = 0;
    QTimer* deathTimer = nullptr;
};
