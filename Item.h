#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPoint>
#include <QString>
#include "SpriteSheetManager.h"


enum class ItemType {
    Needle,
    Turtle,
    SpeedShoes,
    ExtraBomb,
    Glove,
    MoonWalk,
    PowerPotion
};

class Item : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Item(ItemType type, const QPoint& gridPos);

    ItemType getType() const { return type; }
    QString getName() const;
    QPoint getGridPos() const { return gridPos; }
    QPoint getScreenPos() const { return QPoint(gridPos.x() * 50, gridPos.y() * 50); }

private:
    ItemType type;
    QPoint gridPos;
};
