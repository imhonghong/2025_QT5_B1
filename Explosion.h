#pragma once

#include <QObject>
#include <QPoint>
#include <QVector>
#include <QElapsedTimer>
#include <QString>
#include <QPainter>

class BattleScene;

struct ExplosionFlame {
    QPoint pos;
    QString baseKey;
    qint64 bornTime;
};

class Explosion : public QObject {
    Q_OBJECT

public:
    Explosion(QPoint center, BattleScene* scene, int range = 1);
    void tick();                      // 預留未來動畫效果更新
    void draw(QPainter& painter);    // 在 paintEvent 中繪製
    bool isExpired() const;

private:
    QPoint center;
    BattleScene* scene;
    QVector<ExplosionFlame> flames;
    QElapsedTimer timer;
    int duration = 500; // 火焰顯示時間(ms)

    void generateFlames(int range);  // 根據 range 決定圖示與格子
    void applyEffects();             // 擊中玩家、怪、磚塊、水球等（預留）
};
