#pragma once
#include <QPoint>
#include <QPair>
#include <QHash>

inline uint qHash(const QPoint& key, uint seed = 0) {
    return qHash(qMakePair(key.x(), key.y()), seed);
}
