#include "GameControllerMode2.h"
#include <QDebug>
#include "Monster.h"

void GameControllerMode2::initialize(BattleScene* s) {
    scene = s;
    connect(scene, &BattleScene::monsterRemoved, this, [=](Monster* m){
        qDebug() << "[Controller] Remove monster from controller:" << m;
        monsters.removeAll(m);  // ✅ 防止使用到 deleted monster
    });

    loadWave(0);
    connect(player, &Player::requestEndGame, scene, &BattleScene::gameEnded);
}

void GameControllerMode2::reset() {
    currentWave = 0;
    monsters.clear();
    loadWave(currentWave);
}

Player* GameControllerMode2::getPlayer() const {
    return player;
}

void GameControllerMode2::update(float delta) {
    checkWaveCleared();
    if (player) {
        player->update(delta);  // ✅ 加這行
    }

    for (Monster* m : monsters){
        m->updateMovement();
        if (!player->isDead() && m->getCollisionBox().intersects(player->getCollisionBox())) {
            player->takeDamage(1);  // 玩家會自行處理無敵狀態與閃爍
            qDebug() << "[Controller] Player collided with monster:" << m;
        }
    // 可加上時間控制等
    }
    // ✅ 加上 Octopus 判定
    Octopus* o = scene->getOctopus();
    if (o && !o->isDead()) {
        QRect box = o->getCollisionBox();
        QPoint grid = o->getGridPos();

        // 1️⃣ 碰到磚塊
        int tile = scene->getMap(grid);
        if (tile == 1 || tile == 2) {
            scene->setMap(grid, 0);
            qDebug() << "[Octopus] Destroyed tile at" << grid;
        }

        // 2️⃣ 碰到 item
        QVector<Item*>& items = scene->getItems();
        QVector<Item*> toRemove;

        for (Item* item : items) {
            if (!item) continue;
            QRect itemBox(item->getScreenPos(), QSize(50, 50));
            if (itemBox.intersects(box)) {
                toRemove.append(item);
            }
        }

        // 統一處理移除，避免 iterator 被破壞或重複刪除
        for (Item* item : toRemove) {
            scene->removeItem(item);
        }

        // 3️⃣ 碰到 player
        if (!player->isDead() && box.intersects(player->getCollisionBox())) {
            player->takeDamage(1);
            qDebug() << "[Octopus] Hit player!";
        }
    }

    scene->updateOctoBall();
}

void GameControllerMode2::clearMonsters() {
    for (Monster* m : monsters) {
        scene->removeItem(m);
        // delete m;
    }
    monsters.clear();
}

void GameControllerMode2::loadWave(int waveIndex) {
    qDebug() << "[GameControllerMode2] Loading wave" << waveIndex;

    int prevHp = player ? player->getHp() : 3;
    scene->clearScene();

    // 建立新 Player 並保留血量
    player = new Player();
    player->setHp(prevHp);
    player->setController(this);

    // 呼叫對應 wave 初始化
    QPoint playerStart;
    switch (waveIndex) {
        case 0: playerStart = initWave0(); break;
        case 1: playerStart = initWave1(); break;
        case 2: playerStart = initWave2(); break;
    }

    scene->addPlayer(player, playerStart);
}


void GameControllerMode2::checkWaveCleared() {
    bool allDead = true;

    // 判斷 monster 是否全滅
    for (Monster* m : monsters) {
        if (!m->isDead()) {
            allDead = false;
            break;
        }
    }

    // 判斷 octopus 是否死亡（僅 wave2 才有）
    if (currentWave == 2) {
        Octopus* o = scene->getOctopus();
        if (o && !o->isDead()) {
            allDead = false;
        }
    }

    if (allDead) {
        qDebug() << "[GameControllerMode2] wave" << currentWave << " cleared";
        ++currentWave;
        if (currentWave < 3) {
            loadWave(currentWave);
        } else {
            qDebug() << "[GameControllerMode2] All waves cleared!";
            scene->handleGameEnd(true);  // or false
        }
    }
}


void GameControllerMode2::setScene(BattleScene* s) {
    scene = s;
    scene->setController(this);
    scene->setMode(GameMode::Mode2);  // ✅ <--- 新增這行設定 mode 為 Mode2
    qDebug() << "[GameControllerMode2] setScene() → mode = " << static_cast<int>(scene->getMode());
}



QPoint GameControllerMode2::initWave0() {
    currentWave = 0;
    player->setWaveIndex(currentWave);

    QVector<QVector<int>> mapData = {
        {0,0,0,0,0,0,0,0,0,0,0},
        {3,1,3,1,3,1,3,1,3,1,3},
        {0,0,0,0,0,0,0,0,0,0,0},
        {3,1,3,1,3,1,3,1,3,1,3},
        {0,0,0,0,0,0,0,0,0,0,0},
        {3,1,3,1,3,1,3,1,3,1,3},
        {0,0,0,0,0,0,0,0,0,0,0},
        {3,1,3,1,3,1,3,1,3,1,3},
        {0,0,0,0,0,0,0,0,0,0,0},
    };
    scene->setMap(mapData);

    // scene->addBrick(QPoint(3, 1), 1);
    // scene->addBrick(QPoint(7, 1), 1);

    QRect roamZone1(0, 0, 11, 1);
    QRect roamZone2(0, 2, 11, 1);
    QRect roamZone3(0, 4, 11, 1);
    QRect roamZone4(0, 6, 11, 1);
    Monster* m1 = new Monster(QPoint(8, 0), roamZone1, true);
    Monster* m2 = new Monster(QPoint(9, 0), roamZone1, false);
    Monster* m3 = new Monster(QPoint(8, 2), roamZone2, true);
    Monster* m4 = new Monster(QPoint(9, 2), roamZone2, false);
    Monster* m5 = new Monster(QPoint(8, 4), roamZone3, true);
    Monster* m6 = new Monster(QPoint(9, 4), roamZone3, false);
    Monster* m7 = new Monster(QPoint(8, 6), roamZone4, true);
    Monster* m8 = new Monster(QPoint(9, 6), roamZone4, false);
    scene->addMonster(m1);
    monsters.push_back(m1);
    scene->addMonster(m2);
    monsters.push_back(m2);
    scene->addMonster(m3);
    monsters.push_back(m3);
    scene->addMonster(m4);
    monsters.push_back(m4);
    scene->addMonster(m5);
    monsters.push_back(m5);
    scene->addMonster(m6);
    monsters.push_back(m6);
    scene->addMonster(m7);
    monsters.push_back(m7);
    scene->addMonster(m8);
    monsters.push_back(m8);

    return QPoint(5, 8); // player 起始位置
}

QPoint GameControllerMode2::initWave1() {
    currentWave = 1;
    player->setWaveIndex(currentWave);

    QVector<QVector<int>> mapData = {
        {0,0,0,0,0,0,0,0,0,0,0},
        {0,3,3,1,1,1,1,1,3,3,0},
        {0,3,0,0,0,0,0,0,0,3,0},
        {0,1,0,0,1,1,1,0,0,1,0},
        {0,1,0,1,0,0,0,1,0,1,0},
        {0,1,0,0,1,1,1,0,0,1,0},
        {0,3,0,0,0,0,0,0,0,3,0},
        {0,3,3,1,1,1,1,1,3,3,0},
        {0,0,0,0,0,0,0,0,0,0,0},
    };
    scene->setMap(mapData);

    // scene->addBrick(QPoint(3, 1), 1);
    // scene->addBrick(QPoint(7, 1), 1);

    QRect roamZone1(2, 2, 7, 5);
    QRect roamZone2(0, 0, 11, 9);
    Monster* m1 = new Monster(QPoint(2, 2), roamZone1, true);
    Monster* m2 = new Monster(QPoint(2, 6), roamZone1, true);
    Monster* m3 = new Monster(QPoint(8, 2), roamZone1, false);
    Monster* m4 = new Monster(QPoint(8, 6), roamZone1, false);
    Monster* m5 = new Monster(QPoint(0, 0), roamZone2, true);
    Monster* m6 = new Monster(QPoint(10, 0), roamZone2, true);
    Monster* m7 = new Monster(QPoint(0, 8), roamZone2, false);
    Monster* m8 = new Monster(QPoint(10, 8), roamZone2, false);
    scene->addMonster(m1);  monsters.push_back(m1);
    scene->addMonster(m2);  monsters.push_back(m2);
    scene->addMonster(m3);  monsters.push_back(m3);
    scene->addMonster(m4);  monsters.push_back(m4);
    scene->addMonster(m5);  monsters.push_back(m5);
    scene->addMonster(m6);  monsters.push_back(m6);
    scene->addMonster(m7);  monsters.push_back(m7);
    scene->addMonster(m8);  monsters.push_back(m8);


    return QPoint(5, 4); // player 起始位置
}


QPoint GameControllerMode2::initWave2() {
    currentWave = 2;
    player->setWaveIndex(currentWave);

    QVector<QVector<int>> mapData = {
        {7,0,2,0,0,0,0,0,2,0,7},
        {0,2,0,2,0,0,0,2,0,2,0},
        {2,0,0,0,0,0,0,0,0,0,2},
        {0,2,0,0,0,0,0,0,0,2,0},
        {2,0,0,0,0,0,0,0,0,0,2},
        {0,2,0,0,0,0,0,0,0,2,0},
        {2,0,0,0,0,0,0,0,0,0,2},
        {0,2,0,2,0,0,0,2,0,2,0},
        {6,0,2,0,2,0,2,0,2,0,6},
    };
    scene->setMap(mapData);

    // ✅ 創建章魚但先不加到場景
    Octopus* o = new Octopus();

    // ✅ 先設定 player，再加入章魚
    scene->addPlayer(player, QPoint(5, 7));  // 提前設定 player
    o->setPlayer(player);  // 直接設定 player 給章魚
    scene->addOctopus(o);  // 再加入章魚

    return QPoint(5, 7); // player 起始位置（已經設定過了）
}

void GameControllerMode2::clearPlayer() {
    if (player) {
        scene->removeItem(player);
        delete player;
        player = nullptr;
    }
}

void GameControllerMode2::clearItems() {
    scene->clearItems();  // 如果有，否則請遍歷清除 items
}

void GameControllerMode2::nextWave() {
    int prevHp = player ? player->getHp() : 3;

    qDebug() << "[GameControllerMode2] 進入下一關";
    currentWave++;

    clearMonsters();
    clearPlayer();
    clearItems();

    loadWave(currentWave);
    if (player)
        player->setHp(prevHp);
}
