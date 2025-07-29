#include "Octopus.h"
#include "BattleScene.h"

#include <QDebug>

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
    if (isDying) {
        return QString("O_die_%1").arg(qMin(dieFrameIndex, 5));
    }

    QString dir;
    switch (getDirection()) {
        case Direction::Down:  dir = "down"; break;
        case Direction::Left:  dir = "left"; break;
        case Direction::Right: dir = "right"; break;
        case Direction::Up:    dir = "up"; break;
    }
    return QString("O_walk_%1_%2").arg(dir).arg(getFrameIndex());
}


void Octopus::updateAI() {
    if (!scene) {
        qDebug() << "[Octopus] scene is nullptr";
        return;
    }

    if (aiTimer.elapsed() < 3000) return;  // 每 3000ms 更新一次
    aiTimer.restart();

    // ✅ 如果沒有 player，嘗試從 scene 獲取
    if (!player) {
        player = scene->getPlayer();
        if (!player) {
            qDebug() << "[Octopus] Still no player found";
            return;
        } else {
            qDebug() << "[Octopus] Successfully got player from scene";
        }
    }

    qDebug() << "[Octopus] updateAI called";
    QPoint myPos = getGridPos();
    QPoint playerPos = player->getNearestGridPos();

    qDebug() << "[Octopus] My pos:" << myPos << ", Player pos:" << playerPos;

    int dx = playerPos.x() - myPos.x();
    int dy = playerPos.y() - myPos.y();

    // --- 移動邏輯 ---
    if (abs(dx) > abs(dy)) {
        setDirection(dx > 0 ? Direction::Right : Direction::Left);
        QPoint newPos = myPos + QPoint(dx > 0 ? 1 : -1, 0);
        setGridPos(newPos);
        qDebug() << "[Octopus] Moving to:" << newPos;
    } else {
        setDirection(dy > 0 ? Direction::Down : Direction::Up);
        QPoint newPos = myPos + QPoint(0, dy > 0 ? 1 : -1);
        setGridPos(newPos);
        qDebug() << "[Octopus] Moving to:" << newPos;
    }

    // --- 射擊邏輯：與玩家同列/同行就攻擊 ---
    if (dx == 0 || dy == 0) {
        qDebug() << "[Octopus] Attack!";
        OctopusBall* ball = new OctopusBall(getGridPos(), getDirection(), scene);
        scene->addOctopusBall(ball);
    }
}

QRect Octopus::getCollisionBox() const {
    QPointF pos = getScreenPos();
    return QRect(pos.x() + 10, pos.y()+10 + 8, 150-20, 200-30);  // 假設碰撞區比圖像略小
}

void Octopus::onDie() {
    qDebug() << "[Octopus] Died!";
    isDying = true;  // ✅ 讓 getFrameKey() 判斷使用死亡圖
    dieFrameIndex = 1;

    // 不再呼叫 setFrameKey，改由 getFrameKey 控制
    if (!deathTimer) {
        deathTimer = new QTimer(this);
        connect(deathTimer, &QTimer::timeout, this, [=]() {
            dieFrameIndex++;
            if (dieFrameIndex > 5) {
                deathTimer->stop();
                deathTimer->deleteLater();
                deathTimer = nullptr;

                if (scene) {
                    scene->gameEnded(true);  // 玩家勝利
                }
            }
        });
        deathTimer->start(600);
    }
}
