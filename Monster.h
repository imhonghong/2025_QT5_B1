#pragma once
#include "Character.h"
#include <QString>

class Monster : public Character {
public:
    Monster() = default;

    void setMovePattern(const QString& p) { pattern = p; }
    QString getMovePattern() const { return pattern; }

    void updateMovement(); // 預留：依照 pattern 更新座標

private:
    QString pattern; // "clockwise", "counterclockwise", "left-right", "tracker"
};
