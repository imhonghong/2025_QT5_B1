#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>

class EndingScene : public QWidget {
    Q_OBJECT
public:
    EndingScene(QWidget *parent = nullptr);
    void setup(bool isWin);

signals:
    void backToTitle();

private:
    QLabel *resultLabel;
    QPushButton *btnBack;
};
