#include "PathFinder.h"
#include <QSet>
#include <QPair>
#include <QDebug>
#include <QHash>

inline uint qHash(const QPoint& key, uint seed = 0) {
    return qHash(QPair<int, int>(key.x(), key.y()), seed);
}


PathFinder::PathFinder(const QVector<QVector<int>>& map)
    : map(map), rows(map.size()), cols(map[0].size()) {}

bool PathFinder::isValid(QPoint pt) {
    return pt.x() >= 0 && pt.x() < cols && pt.y() >= 0 && pt.y() < rows;
}

int PathFinder::getCost(QPoint pt) {
    int val = map[pt.y()][pt.x()];
    if (val == 0 || val == 4 || val == 5) return 1;     // 空地 / player / robot 起點
    if (val == 1 || val == 2) return 5;                 // 可爆破磚塊 = 1(移動) + 4(爆破)
    return 9999;                                        // 不可走
}

QVector<QPoint> PathFinder::bfs(QPoint start, QPoint end) {
    QHash<QPoint, int> dist;
    QHash<QPoint, QPoint> prev;
    QQueue<QPoint> q;
    QSet<QPoint> visited;

    q.enqueue(start);
    dist[start] = 0;

    QVector<QPoint> directions = {
        QPoint(1, 0), QPoint(-1, 0), QPoint(0, 1), QPoint(0, -1)
    };

    while (!q.isEmpty()) {
        QPoint curr = q.dequeue();
        if (curr == end) break;

        for (const QPoint& d : directions) {
            QPoint next = curr + d;
            if (!isValid(next)) continue;
            int cost = getCost(next);
            if (cost >= 9999) continue;

            int newDist = dist[curr] + cost;
            if (!dist.contains(next) || newDist < dist[next]) {
                dist[next] = newDist;
                prev[next] = curr;
                q.enqueue(next);
            }
        }
    }

    if (!prev.contains(end)) return {}; // 無路徑

    // 回推路徑
    QVector<QPoint> path;
    QPoint step = end;
    while (step != start) {
        path.prepend(step);
        step = prev[step];
    }
    path.prepend(start);
    return path;
}

PathResult PathFinder::findPath(QPoint start, QPoint player) {
    QVector<QPoint> goals = {
        player + QPoint(1, 0),
        player + QPoint(-1, 0),
        player + QPoint(0, 1),
        player + QPoint(0, -1)
    };

    // 遍歷候選終點，挑出最佳路徑
    QVector<QPoint> bestPath;
    int minCost = 999999;

    for (const QPoint& g : goals) {
        if (!isValid(g)) continue;
        if (getCost(g) >= 9999) continue;

        QVector<QPoint> p = bfs(start, g);  // 👈 呼叫你封裝的 bfs()
        if (!p.isEmpty() && p.size() < minCost) {
            bestPath = p;
            minCost = p.size();
        }
    }

    // 掃描路徑中是否有需要炸的牆
    PathResult result;
    result.path = bestPath;

    for (int i = 0; i < bestPath.size() - 1; ++i) {
        QPoint curr = bestPath[i];
        QPoint next = bestPath[i + 1];

        if (isValid(next) && map[next.y()][next.x()] == 1) {
            result.bombSpots.append(curr);
        }
    }
    result.bombSpots.append(bestPath[bestPath.size()-1]);

    return result;
}
