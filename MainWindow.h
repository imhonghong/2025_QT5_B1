#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "TitleScene.h"
#include "BattleScene.h"
#include "EndingScene.h"
#include "GameControllerMode1.h"
#include "GameControllerMode2.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    void pauseGame();
private slots:
    void startGame(GameMode mode);
    void backToTitle();
    void showEnding(bool isWin);
private:
    QStackedWidget *stack;
    TitleScene *titleScene;
    BattleScene *battleScene;
    EndingScene *endingScene;
};
