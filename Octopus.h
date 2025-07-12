#pragma once
#include "Character.h"

class Octopus : public Character {
public:
    Octopus() = default;

    void hit(); // 被炸彈打中一次
    bool isDead() const;

    void updateAI(); // 預留：依照玩家位置移動/攻擊

private:
    int hp = 6; // 被打中 6 次死亡
};
