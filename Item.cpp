#include "Item.h"
#include <QDebug>

Item::Item(ItemType type, const QPoint& gridPos)
    : QObject(), QGraphicsPixmapItem(), type(type), gridPos(gridPos) {
    setPos(gridPos.x() * 50, gridPos.y() * 50);
}

QString Item::getName() const {
    switch (type) {
        case ItemType::Needle:      return "Needle";
        case ItemType::Turtle:      return "Turtle";
        case ItemType::SpeedShoes:  return "SpeedShoes";
        case ItemType::ExtraBomb:   return "ExtraBomb";
        case ItemType::Glove:       return "Glove";
        case ItemType::MoonWalk:    return "MoonWalk";
        case ItemType::PowerPotion:     return "PowerPotion";
        default:                    return "Turtle";
    }
}
