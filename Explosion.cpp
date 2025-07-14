#include "Explosion.h"
#include "SpriteSheetManager.h"
#include "BattleScene.h"
#include <QDebug>

inline QPoint gridToScreen(const QPoint& gridPos) {
    return QPoint(gridPos.x() * 50, gridPos.y() * 50);
}

Explosion::Explosion(QPoint center, BattleScene* scene, int range)
    : center(center), scene(scene) {
    timer.start();
    generateFlames(range);
    applyEffects();
}

bool Explosion::isExpired() const {
    return timer.elapsed() > duration;
}

void Explosion::tick() {
    // 如果有火焰動畫變化邏輯，可以放這裡
}

void Explosion::draw(QPainter& painter) {
    qint64 now = timer.elapsed();
    for (const auto& f : flames) {
        int frame = 1 + (now - f.bornTime) / 500 % 4;  // 切換速度可調整
        QString spriteKey = QString("%1_%2").arg(f.baseKey).arg(frame);  // e.g. WB_stem_up_3
        QPixmap pix = SpriteSheetManager::instance().getFrame(spriteKey);
        QPoint screenPos = gridToScreen(f.pos);
        QRect targetRect(screenPos.x(), screenPos.y(), 50, 50);
        painter.drawPixmap(targetRect, pix);
    }
}


void Explosion::generateFlames(int range) {
    qint64 now = timer.elapsed();

    // 中心火焰（不變）
    flames.push_back({ center, "WB_center", now });

    struct Dir {
        QString name;
        QPoint delta;
    };

    QVector<Dir> directions = {
        { "up",    QPoint(0, -1) },
        { "down",  QPoint(0,  1) },
        { "left",  QPoint(-1, 0) },
        { "right", QPoint(1,  0) }
    };

    // 四個方向逐格延伸
    for (const Dir& dir : directions) {
        for (int i = 1; i <= range; ++i) {
            QPoint p = center + dir.delta * i;
            QString part = (i == range) ? "end" : "stem";  // 最遠端用 end，其餘用 stem
            QString baseKey = QString("WB_%1_%2").arg(part, dir.name);  // 不含 frame 編號
            flames.push_back({ p, baseKey, now });
        }
    }
}

void Explosion::applyEffects() {
    for (const auto& f : flames) {
        // 可擴充功能：
        // - 擊中 Player
        // - 擊中 Monster
        // - 擊中可炸磚
        // - 引爆水球
        // ➕ 你可透過 scene->getMap()[y][x] 之類進行處理
        qDebug() << "[Explosion] 火焰於" << f.pos;
    }
}
