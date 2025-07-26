#pragma once
#include <QString>
#include <QPoint>

enum class ItemType {
    Needle,
    Turtle,
    SpeedShoes,
    ExtraBomb,
    Glove,
    MoonWalk,
    PowerPotion
};

class Item {
public:
    Item(ItemType type, const QPoint& gridPos)
        : type(type), gridPos(gridPos) {}

    ItemType getType() const { return type; }
    QString getName() const;  // 取得文字名稱，對應圖片名稱
    QPoint getGridPos() const { return gridPos; }
    QPoint getScreenPos() const { return QPoint(gridPos.x() * 50, gridPos.y() * 50); }

private:
    ItemType type;
    QPoint gridPos;
};
