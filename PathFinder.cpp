#include "PathFinder.h"
#include <QSet>
#include <QPair>
#include <QDebug>
#include <QHash>
#include "HashUtil.h"

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

QVector<QPoint> PathFinder::findPath(QPoint start, QPoint end) {
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

bool PathFinder::findNextBombPlan(QPoint start, QPoint& bombPoint, QVector<QPoint>& pathToBomb) {
    QVector<QPoint> candidates;

    static const QVector<QPoint> directions = {
        QPoint(0, 1), QPoint(0, -1),
        QPoint(1, 0), QPoint(-1, 0)
    };

    // 找所有炸牆點（可移動空格，且鄰近有牆）
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            QPoint pt(x, y);
            if (map[y][x] != 0) continue;
            for (const QPoint& dir : directions) {
                QPoint neighbor = pt + dir;
                if (isValid(neighbor) && map[neighbor.y()][neighbor.x()] == 1) {
                    candidates.push_back(pt);
                    break;
                }
            }
        }
    }

    int minCost = 999999;
    QVector<QPoint> bestPath;
    QPoint bestBomb;

    for (const QPoint& cand : candidates) {
        QVector<QPoint> path = findPath(start, cand);
        if (path.isEmpty()) continue;

        if (path.size() < minCost) {
            minCost = path.size();
            bestPath = path;
            bestBomb = cand;
        }
    }

    if (!bestPath.isEmpty()) {
        bombPoint = bestBomb;
        pathToBomb = bestPath;
        return true;
    }

    return false;
}
