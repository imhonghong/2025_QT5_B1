#pragma once

#include <QVector>
#include <QPoint>
#include <QQueue>
#include <QMap>

class PathFinder {
public:
    PathFinder(const QVector<QVector<int>>& map);

    QVector<QPoint> findPath(QPoint start, QPoint end); // 回傳從 start 到 end 的最短路徑

private:
    QVector<QVector<int>> map;
    int rows, cols;

    bool isValid(QPoint pt);
    int getCost(QPoint pt);
};
