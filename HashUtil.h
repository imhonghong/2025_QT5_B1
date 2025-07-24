#pragma once
#include <QPoint>
#include <QPair>
#include <QHash>
#include "Monster.h"
#include "Octopus.h"

inline uint qHash(const QPoint& key, uint seed = 0) {
    return qHash(qMakePair(key.x(), key.y()), seed);
}

inline uint qHash(Monster* const& key, uint seed = 0) {
    return qHash(reinterpret_cast<quintptr>(key), seed);
}

inline uint qHash(Octopus* const& key, uint seed = 0) {
    return qHash(reinterpret_cast<quintptr>(key), seed);
}

