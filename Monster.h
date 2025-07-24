#pragma once
#include <QObject>
#include "Character.h"
#include <QString>
#include "QPainter"

class BattleScene;
class Monster : public QObject, public Character {
    Q_OBJECT

public:
    Monster(const QPoint& pos);
    Monster(const QPoint& pos, const QRect& roamZone, bool clockwise);

    QString getFrameKey() const override;
    void paint(QPainter* painter);
    void setScene(BattleScene* s) { scene = s; }

    void setMovePattern(const QString& p) { pattern = p; }
    QString getMovePattern() const { return pattern; }

    void updateMovement();
    void onDie() override;

signals:
    void requestDelete(Monster*);
private:

    BattleScene* scene = nullptr;
    QString pattern;
    float moveSpeed = 0.02f;
    QRect roamZone;
    bool clockwise = true;
    Direction currentDir = Direction::Right;
    QPoint gridTarget;
    bool isMoving = false;

    bool isOnEdge(const QPoint& p) const;
    Direction nextDirection(Direction dir, bool cw) const;
    void initStartDirection(const QPoint& pos);

    float frameTimer = 0.0f;         // 幀更新累積時間
    float frameInterval = 0.2f;      // 每 0.2 秒切一次動畫
    QTimer* frameUpdateTimer = nullptr;

    bool isDying = false;
    QTimer* deathTimer = nullptr;
};
