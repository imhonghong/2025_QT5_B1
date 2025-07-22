QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BattleScene.cpp \
    EndingScene.cpp \
    Explosion.cpp \
    GameControllerMode1.cpp \
    GameControllerMode2.cpp \
    MapLoader.cpp \
    Monster.cpp \
    Octopus.cpp \
    PathFinder.cpp \
    Player.cpp \
    Robot.cpp \
    SpriteSheetManager.cpp \
    TitleScene.cpp \
    WaterBomb.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    BattleScene.h \
    Character.h \
    EndingScene.h \
    Explosion.h \
    GameControllerMode1.h \
    GameControllerMode2.h \
    GameMode.h \
    HashUtil.h \
    IGameController.h \
    MainWindow.h \
    MapLoader.h \
    Monster.h \
    Octopus.h \
    PathFinder.h \
    Player.h \
    Robot.h \
    SpriteSheetManager.h \
    TitleScene.h \
    WaterBomb.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    B1_zh_TW.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    data.qrc
