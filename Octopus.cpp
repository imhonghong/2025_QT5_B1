#include "Octopus.h"

Octopus::Octopus() {
    setDirection(Direction::Down);
    setHp(6);
}

void Octopus::hit() {
    hp--;
    if (hp <= 0) setHp(0);
}

bool Octopus::isDead() const {
    return hp <= 0;
}

QString Octopus::getFrameKey() const {
    QString dir;
    switch (getDirection()) {
        case Direction::Down:  dir = "down"; break;
        case Direction::Left:  dir = "left"; break;
        case Direction::Right: dir = "right"; break;
        case Direction::Up:    dir = "up"; break;
    }
    return QString("O_walk_%1_%2").arg(dir).arg(getFrameIndex());
}
