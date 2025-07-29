#include "MainWindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setFixedSize(550, 450 + 100); // 遊戲區 + UI 區

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    titleScene = new TitleScene(this);
    battleScene = new BattleScene(this);
    endingScene = new EndingScene(this);

    stack->addWidget(titleScene);
    stack->addWidget(battleScene);
    stack->addWidget(endingScene);

    connect(titleScene, &TitleScene::startGame, this, &MainWindow::startGame);
    connect(battleScene, &BattleScene::backToTitle, this, &MainWindow::backToTitle);
    connect(battleScene, &BattleScene::gameEnded, this, &MainWindow::showEnding);
    connect(endingScene, &EndingScene::backToTitle, this, &MainWindow::backToTitle);
    connect(battleScene, &BattleScene::pauseRequested, this, &MainWindow::pauseGame);
    connect(battleScene, &BattleScene::returnToMainMenu, this, &MainWindow::backToTitle);

    stack->setCurrentWidget(titleScene);
}

void MainWindow::startGame(GameMode mode) {
    qDebug() << "[MainWindow] Starting game with mode:" << static_cast<int>(mode);

    // ✅ 1. 徹底清除現有場景內容
    battleScene->clearScene();

    // ✅ 2. 確保計時器已停止
    if (battleScene->updateTimer.isActive()) {
        battleScene->updateTimer.stop();
        qDebug() << "[MainWindow] Stopped existing timer";
    }

    // ✅ 3. 建立新的 Controller
    IGameController* controller = nullptr;
    if (mode == GameMode::Mode1) {
        controller = new GameControllerMode1();
    } else {
        controller = new GameControllerMode2();
        battleScene->setMode(GameMode::Mode2);
    }

    // ✅ 4. 設定 controller 並切換畫面
    battleScene->setController(controller);
    stack->setCurrentWidget(battleScene);

    qDebug() << "[MainWindow] Game started successfully";
}


void MainWindow::backToTitle() {
    stack->setCurrentWidget(titleScene);
}

void MainWindow::showEnding(bool isWin) {
    endingScene->setup(isWin);
    stack->setCurrentWidget(endingScene);
}

void MainWindow::pauseGame() {
    battleScene->togglePause();  // 切換是否暫停
}
