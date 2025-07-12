#include "TitleScene.h"
#include <QApplication>

TitleScene::TitleScene(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QPushButton *btnMode1 = new QPushButton("Start Mode 1 (Robot)", this);
    QPushButton *btnMode2 = new QPushButton("Start Mode 2 (Monsters)", this);
    QPushButton *btnExit = new QPushButton("Exit", this);

    layout->addWidget(btnMode1);
    layout->addWidget(btnMode2);
    layout->addWidget(btnExit);

    connect(btnMode1, &QPushButton::clicked, this, [=](){
        emit startGame(GameMode::Mode1);
    });

    connect(btnMode2, &QPushButton::clicked, this, [=](){
        emit startGame(GameMode::Mode2);
    });

    connect(btnExit, &QPushButton::clicked, this, [=](){
        qApp->quit();
    });
}
