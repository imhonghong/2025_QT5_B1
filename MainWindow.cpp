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
    if (mode == GameMode::Mode1) {
        IGameController* controller = new GameControllerMode1();
        battleScene->setController(controller);
        stack->setCurrentWidget(battleScene);
    } else {
        IGameController* controller = new GameControllerMode2();  // ✅ 使用你剛實作的 controller
        battleScene->setController(controller);
        battleScene->setMode(GameMode::Mode2);
        stack->setCurrentWidget(battleScene);
    }
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
