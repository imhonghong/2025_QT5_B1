// MapLoader.h
#pragma once
#include <QString>
#include <QVector>

class MapLoader {
public:
    static QVector<QVector<int>> loadMap(const QString &filePath);
};
