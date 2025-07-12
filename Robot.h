#pragma once
#include "Character.h"

class Robot : public Character {
public:
    Robot() = default;

    void setStepCount(int step) { stepCount = step; }
    int getStepCount() const { return stepCount; }

    void updateAI(); // 預留：讓 robot 執行尋路、炸彈邏輯

private:
    int stepCount = 0;
};
