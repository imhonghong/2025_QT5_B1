#include "Item.h"

QString Item::getName() const {
    switch (type) {
        case ItemType::Needle:      return "Needle";
        case ItemType::Turtle:      return "Turtle";
        case ItemType::SpeedShoes:  return "SpeedShoes";
        case ItemType::ExtraBomb:   return "ExtraBomb";
        case ItemType::Glove:       return "Glove";
        case ItemType::MoonWalk:    return "MoonWalk";
        case ItemType::PowerPotion:     return "PowerPotion";
        default:                    return "UnknownItem";
    }
}
