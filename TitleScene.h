#pragma once
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "GameController.h"

class TitleScene : public QWidget {
    Q_OBJECT
public:
    TitleScene(QWidget *parent = nullptr);
signals:
    void startGame(GameMode mode);
};
