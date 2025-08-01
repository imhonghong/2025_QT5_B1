#include "Explosion.h"
#include "SpriteSheetManager.h"
#include "BattleScene.h"
#include <QDebug>
#include <QRandomGenerator>

inline QPoint gridToScreen(const QPoint& gridPos) {
    return QPoint(gridPos.x() * 50, gridPos.y() * 50);
}

Explosion::Explosion(QPoint center, BattleScene* scene, int range)
    : QObject(scene), center(center), scene(scene), range(range) {
    timer.start();
    generateFlames(range);
    applyEffects();
}

bool Explosion::isExpired() const {
    return timer.elapsed() > duration;
}

void Explosion::tick() {
    if (!hasAppliedEffect) {
        applyEffects();
        hasAppliedEffect = true;
    }
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
            int tile = scene->getMap(p);

            // ⛔ 若是牆，完全無法穿透
            if (tile == 3)
                break;

            QString part = (i == range) ? "end" : "stem";
            QString baseKey = QString("WB_%1_%2").arg(part, dir.name);
            flames.push_back({ p, baseKey, now });

            // ⛔ 若是磚，炸掉後終止延伸
            if (tile == 1 || tile == 2)
                break;
        }
    }
}

void Explosion::applyEffects() {

    Player* player = scene->getPlayer();              // mode2
    Robot* robot = scene->getRobot();                 // mode1
    const QVector<Monster*>& monsters = scene->getMonsters();
    Octopus* octopus = scene->getOctopus();

    if (hasAppliedEffect) return;
    hasAppliedEffect = true;

    // 玩家處理
    if (Player* p = scene->getPlayer()) {
        qDebug() << "[Explosion] applyEffects triggered";
        QRect playerBox = p->getCollisionBox();
        bool octopusHit = false;

        for (const ExplosionFlame& f : flames) {
            QPoint p = f.pos;
            QRect flameRect = QRect(gridToScreen(p), QSize(50, 50));

            // 🎯 Robot
            if (robot && robot->getGridPos() == p) {
                qDebug() << "[Explosion] Robot 被炸死 at" << p;
                robot->takeDamage(1);
                // 之後可呼叫 robot->die()
            }
            // 🎯 Player
            if (playerBox.intersects(flameRect)) {
                bool effectRemoved = false;

                if (player->hasItem(ItemType::Turtle)) {
                    player->onTurtleBreak();
                    qDebug() << "[Explosion] 玩家炸到烏龜壞了 -> 清除 turtle + 無敵";
                    effectRemoved = true;
                    break;

                }

                if (player->hasItem(ItemType::SpeedShoes)) {
                    player->removeItem(ItemType::SpeedShoes);
                    effectRemoved = true;
                    qDebug() << "[Explosion] 玩家被炸，移除 SpeedShoes";
                }

                if (effectRemoved) {
                    player->updateMoveSpeed();   // ✅ 重新計算速度
                    player->setStateStanding();  // ✅ 回復正常狀態
                    qDebug() << "[Explosion] 玩家被炸，狀態與速度重置";
                } else {
                    player->enterTrappedState(); // 原本邏輯
                    qDebug() << "[Explosion] 玩家被炸到，進入 Trapped";
                }

                break; // ✅ 僅炸一次
            }
            // 🎯 Monsters
            for (Monster* m : monsters) {
                if (m && m->getCollisionBox().intersects(flameRect)) {
                    m->takeDamage(1);
                    qDebug() << "[Explosion] Monster takeDamage(1) at" << p;
                    // 之後可呼叫 m->die()
                }
            }
            // 🎯 Octopi
            if (octopus && !octopusHit && octopus->getCollisionBox().intersects(flameRect)) {
                octopus->takeDamage(1);
                octopusHit = true;
                qDebug() << "[Explosion] Octopus take 1 damage";
            }

            // 若是磚塊 → 爆破
            if (scene->getMap(p) == 1 || scene->getMap(p) == 2) {
                scene->setMap(p, 0);
                if (scene->getMode() == GameMode::Mode2){
                    int r = QRandomGenerator::global()->bounded(7);
                    ItemType type = static_cast<ItemType>(r);
                    Item* item = new Item(type, p);
                    scene->addItem(item);
                    qDebug() << "[Explosion] 爆炸產生 item" << item->getName() << "at" << p;
                }
            }
            // 💣 引爆水球
            for (WaterBomb* bomb : scene->getWaterBombs()) {
                if (!bomb->getHasExploded() && bomb->getGridPos() == p) {
                    qDebug() << "[Explosion] 引爆水球 at" << p;
                    QTimer::singleShot(250, bomb, [bomb]() {
                        bomb->explode();  // 延遲 0.25 秒觸發爆炸
                    });
                }
            }
        }   // for Flame end
    } //player end
}
