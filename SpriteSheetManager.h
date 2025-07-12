#pragma once
#include <QPixmap>
#include <QMap>
#include <QString>

/**
 * SpriteSheetManager:
 * 單例類別，用於一次性載入所有已裁切圖片並提供 getFrame() 快速取得。
 */

class SpriteSheetManager {
public:
    static SpriteSheetManager& instance();

    void loadAllSprites(); // 在 main.cpp 程式啟動時呼叫一次載入所有圖片

    QPixmap getFrame(const QString &frameName) const;

private:
    SpriteSheetManager() {} // 禁止外部建構
    SpriteSheetManager(const SpriteSheetManager&) = delete;
    SpriteSheetManager& operator=(const SpriteSheetManager&) = delete;

    QMap<QString, QPixmap> framePixmaps;
};
