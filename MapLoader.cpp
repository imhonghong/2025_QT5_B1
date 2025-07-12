// MapLoader.cpp
#include "MapLoader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

QVector<QVector<int>> MapLoader::loadMap(const QString &filePath) {
    QVector<QVector<int>> mapData;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open map file:" << filePath;
        return mapData;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList nums = line.split(QRegExp("\\s+"));
        QVector<int> row;
        for (QString num : nums) {
            row.append(num.toInt());
        }
        mapData.append(row);
    }

    file.close();
    return mapData;
}
