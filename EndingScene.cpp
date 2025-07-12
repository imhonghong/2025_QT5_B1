#include "EndingScene.h"
#include <QVBoxLayout>

EndingScene::EndingScene(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    resultLabel = new QLabel("Result", this);
    btnBack = new QPushButton("Back to Title", this);

    layout->addWidget(resultLabel);
    layout->addWidget(btnBack);

    connect(btnBack, &QPushButton::clicked, this, [=](){
        emit backToTitle();
    });
}

void EndingScene::setup(bool isWin) {
    if (isWin) {
        resultLabel->setText("You Win!");
    } else {
        resultLabel->setText("Game Over!");
    }
}
