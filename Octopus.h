#pragma once
#include <QObject>
#include "Character.h"

class Octopus : public QObject, public Character {
    Q_OBJECT

public:
    Octopus();
    void hit(); // 被炸彈打中一次
    bool isDead() const override;
    void updateAI(); // 預留：依照玩家位置移動/攻擊

    QString getFrameKey() const override;

private:
    int hp = 6;
};
