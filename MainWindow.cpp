#include "MainWindow.h"
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

    stack->setCurrentWidget(titleScene);
}

void MainWindow::startGame(GameMode mode) {
    battleScene->setup(mode);
    stack->setCurrentWidget(battleScene);
}

void MainWindow::backToTitle() {
    stack->setCurrentWidget(titleScene);
}

void MainWindow::showEnding(bool isWin) {
    endingScene->setup(isWin);
    stack->setCurrentWidget(endingScene);
}
