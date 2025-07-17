#pragma once

#include <QVector>
#include <QPoint>
#include <QQueue>
#include <QMap>

struct PathResult { // 打包路徑和炸彈點
    QVector<QPoint> path;
    QVector<QPoint> bombSpots;
};

class PathFinder {
public:
    PathFinder(const QVector<QVector<int>>& map);
    PathResult findPath(QPoint start, QPoint player); //計算 robot 到 player 周圍


private:
    QVector<QVector<int>> map;
    int rows, cols;

    bool isValid(QPoint pt);
    int getCost(QPoint pt);
    QVector<QPoint> bfs(QPoint start, QPoint end); // 尋路 helper
};
