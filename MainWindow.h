#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "TitleScene.h"
#include "BattleScene.h"
#include "EndingScene.h"
#include "GameController.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
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
