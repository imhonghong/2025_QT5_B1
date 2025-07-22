#include "SpriteSheetManager.h"
#include <QDebug>

SpriteSheetManager& SpriteSheetManager::instance() {
    static SpriteSheetManager manager;
    return manager;
}

void SpriteSheetManager::loadAllSprites() {
    // brick
    framePixmaps.insert("brick_0", QPixmap(":/brick/data/brick/brick_0.png"));
    framePixmaps.insert("brick_1", QPixmap(":/brick/data/brick/brick_1.png"));
    framePixmaps.insert("brick_2", QPixmap(":/brick/data/brick/brick_2.png"));
    framePixmaps.insert("brick_3_0", QPixmap(":/brick/data/brick/brick_3_0.png"));
    framePixmaps.insert("brick_3_1", QPixmap(":/brick/data/brick/brick_3_1.png"));
    framePixmaps.insert("brick_6", QPixmap(":/brick/data/brick/brick_6.png"));
    framePixmaps.insert("brick_7", QPixmap(":/brick/data/brick/brick_7.png"));
    //item
    framePixmaps.insert("extra_water_ball", QPixmap(":/item/data/item/extra_water_balloons.png"));
    framePixmaps.insert("glove", QPixmap(":/item/data/item/glove.png"));
    framePixmaps.insert("moon_walk", QPixmap(":/item/data/item/moon_walk.png"));
    framePixmaps.insert("needle", QPixmap(":/item/data/item/needle.png"));
    framePixmaps.insert("power_potion", QPixmap(":/item/data/item/power_potion.png"));
    framePixmaps.insert("speed_shoes", QPixmap(":/item/data/item/speed_shoes.png"));
    framePixmaps.insert("turtle", QPixmap(":/item/data/item/turtle.png"));
    //moster
    framePixmaps.insert("M_die_1", QPixmap(":/Monster/data/character/M_die_1.png"));
    framePixmaps.insert("M_die_2", QPixmap(":/Monster/data/character/M_die_2.png"));
    framePixmaps.insert("M_die_3", QPixmap(":/Monster/data/character/M_die_3.png"));
    framePixmaps.insert("M_die_4", QPixmap(":/Monster/data/character/M_die_4.png"));
    framePixmaps.insert("M_walk_down_1", QPixmap(":/Monster/data/character/M_walk_down_1.png"));
    framePixmaps.insert("M_walk_down_2", QPixmap(":/Monster/data/character/M_walk_down_2.png"));
    framePixmaps.insert("M_walk_left_1", QPixmap(":/Monster/data/character/M_walk_left_1.png"));
    framePixmaps.insert("M_walk_left_2", QPixmap(":/Monster/data/character/M_walk_left_2.png"));
    framePixmaps.insert("M_walk_right_1", QPixmap(":/Monster/data/character/M_walk_right_1.png"));
    framePixmaps.insert("M_walk_right_2", QPixmap(":/Monster/data/character/M_walk_right_2.png"));
    framePixmaps.insert("M_walk_up_1", QPixmap(":/Monster/data/character/M_walk_up_1.png"));
    framePixmaps.insert("M_walk_up_2", QPixmap(":/Monster/data/character/M_walk_up_2.png"));
    //octopus
    framePixmaps.insert("O_die_1", QPixmap(":/Octopus/data/character/O_die_1.png"));
    framePixmaps.insert("O_die_2", QPixmap(":/Octopus/data/character/O_die_2.png"));
    framePixmaps.insert("O_die_3", QPixmap(":/Octopus/data/character/O_die_3.png"));
    framePixmaps.insert("O_die_4", QPixmap(":/Octopus/data/character/O_die_4.png"));
    framePixmaps.insert("O_die_5", QPixmap(":/Octopus/data/character/O_die_5.png"));
    framePixmaps.insert("O_walk_down_1", QPixmap(":/Octopus/data/character/O_walk_down_1.png"));
    framePixmaps.insert("O_walk_left_1", QPixmap(":/Octopus/data/character/O_walk_left_1.png"));
    framePixmaps.insert("O_walk_right_1", QPixmap(":/Octopus/data/character/O_walk_right_1.png"));
    framePixmaps.insert("O_walk_up_1", QPixmap(":/Octopus/data/character/O_walk_up_1.png"));
    //player-stand
    framePixmaps.insert("P_stand_down_1",QPixmap(":/Player/data/character/P_stand_down_1.png"));
    framePixmaps.insert("P_stand_left_1",QPixmap(":/Player/data/character/P_stand_left_1.png"));
    framePixmaps.insert("P_stand_right_1",QPixmap(":/Player/data/character/P_stand_right_1.png"));
    framePixmaps.insert("P_stand_up_1",QPixmap(":/Player/data/character/P_stand_up_1.png"));
    //player-walk
    framePixmaps.insert("P_walk_down_1", QPixmap(":/Player/data/character/P_walk_down_1.png"));
    framePixmaps.insert("P_walk_down_2", QPixmap(":/Player/data/character/P_walk_down_2.png"));
    framePixmaps.insert("P_walk_down_3", QPixmap(":/Player/data/character/P_walk_down_3.png"));
    framePixmaps.insert("P_walk_down_4", QPixmap(":/Player/data/character/P_walk_down_4.png"));
    framePixmaps.insert("P_walk_left_1", QPixmap(":/Player/data/character/P_walk_left_1.png"));
    framePixmaps.insert("P_walk_left_2", QPixmap(":/Player/data/character/P_walk_left_2.png"));
    framePixmaps.insert("P_walk_left_3", QPixmap(":/Player/data/character/P_walk_left_3.png"));
    framePixmaps.insert("P_walk_left_4", QPixmap(":/Player/data/character/P_walk_left_4.png"));
    framePixmaps.insert("P_walk_right_1", QPixmap(":/Player/data/character/P_walk_right_1.png"));
    framePixmaps.insert("P_walk_right_2", QPixmap(":/Player/data/character/P_walk_right_2.png"));
    framePixmaps.insert("P_walk_right_3", QPixmap(":/Player/data/character/P_walk_right_3.png"));
    framePixmaps.insert("P_walk_right_4", QPixmap(":/Player/data/character/P_walk_right_4.png"));
    framePixmaps.insert("P_walk_up_1", QPixmap(":/Player/data/character/P_walk_up_1.png"));
    framePixmaps.insert("P_walk_up_2", QPixmap(":/Player/data/character/P_walk_up_2.png"));
    framePixmaps.insert("P_walk_up_3", QPixmap(":/Player/data/character/P_walk_up_3.png"));
    framePixmaps.insert("P_walk_up_4", QPixmap(":/Player/data/character/P_walk_up_4.png"));
    //player-waterball/needle/die
    framePixmaps.insert("P_wb_1", QPixmap(":/Player/data/character/P_wb_1.png"));
    framePixmaps.insert("P_wb_2", QPixmap(":/Player/data/character/P_wb_2.png"));
    framePixmaps.insert("P_wb_3", QPixmap(":/Player/data/character/P_wb_3.png"));
    framePixmaps.insert("P_wb_4", QPixmap(":/Player/data/character/P_wb_4.png"));
    framePixmaps.insert("P_wd_1", QPixmap(":/Player/data/character/P_wd_1.png"));
    framePixmaps.insert("P_wd_2", QPixmap(":/Player/data/character/P_wd_2.png"));
    framePixmaps.insert("P_wn_1", QPixmap(":/Player/data/character/P_wn_1.png"));
    framePixmaps.insert("P_wn_2", QPixmap(":/Player/data/character/P_wn_2.png"));
    framePixmaps.insert("P_die", QPixmap(":/Player/data/character/P_die.png"));
    //player-turtle
    framePixmaps.insert("PT_walk_down_1", QPixmap(":/PlayerTurtule/data/character/PT_walk_down_1.png"));
    framePixmaps.insert("PT_walk_down_2", QPixmap(":/PlayerTurtule/data/character/PT_walk_down_2.png"));
    framePixmaps.insert("PT_walk_left_1", QPixmap(":/PlayerTurtule/data/character/PT_walk_left_1.png"));
    framePixmaps.insert("PT_walk_left_2", QPixmap(":/PlayerTurtule/data/character/PT_walk_left_2.png"));
    framePixmaps.insert("PT_walk_right_1", QPixmap(":/PlayerTurtule/data/character/PT_walk_right_1.png"));
    framePixmaps.insert("PT_walk_right_2", QPixmap(":/PlayerTurtule/data/character/PT_walk_right_2.png"));
    framePixmaps.insert("PT_walk_up_1", QPixmap(":/PlayerTurtule/data/character/PT_walk_up_1.png"));
    framePixmaps.insert("PT_walk_up_2", QPixmap(":/PlayerTurtule/data/character/PT_walk_up_2.png"));
    //robot
    framePixmaps.insert("R_stand_down_1", QPixmap(":/Robot/data/character/R_stand_down_1.png"));
    framePixmaps.insert("R_stand_left_1", QPixmap(":/Robot/data/character/R_stand_left_1.png"));
    framePixmaps.insert("R_stand_right_1", QPixmap(":/Robot/data/character/R_stand_right_1.png"));
    framePixmaps.insert("R_stand_up_1", QPixmap(":/Robot/data/character/R_stand_up_1.png"));
    framePixmaps.insert("R_walk_down_1", QPixmap(":/Robot/data/character/R_walk_down_1.png"));
    framePixmaps.insert("R_walk_down_2", QPixmap(":/Robot/data/character/R_walk_down_2.png"));
    framePixmaps.insert("R_walk_down_3", QPixmap(":/Robot/data/character/R_walk_down_3.png"));
    framePixmaps.insert("R_walk_down_4", QPixmap(":/Robot/data/character/R_walk_down_4.png"));
    framePixmaps.insert("R_walk_left_1", QPixmap(":/Robot/data/character/R_walk_left_1.png"));
    framePixmaps.insert("R_walk_left_2", QPixmap(":/Robot/data/character/R_walk_left_2.png"));
    framePixmaps.insert("R_walk_left_3", QPixmap(":/Robot/data/character/R_walk_left_3.png"));
    framePixmaps.insert("R_walk_left_4", QPixmap(":/Robot/data/character/R_walk_left_4.png"));
    framePixmaps.insert("R_walk_right_1", QPixmap(":/Robot/data/character/R_walk_right_1.png"));
    framePixmaps.insert("R_walk_right_2", QPixmap(":/Robot/data/character/R_walk_right_2.png"));
    framePixmaps.insert("R_walk_right_3", QPixmap(":/Robot/data/character/R_walk_right_3.png"));
    framePixmaps.insert("R_walk_right_4", QPixmap(":/Robot/data/character/R_walk_right_4.png"));
    framePixmaps.insert("R_walk_up_1", QPixmap(":/Robot/data/character/R_walk_up_1.png"));
    framePixmaps.insert("R_walk_up_2", QPixmap(":/Robot/data/character/R_walk_up_2.png"));
    framePixmaps.insert("R_walk_up_3", QPixmap(":/Robot/data/character/R_walk_up_3.png"));
    framePixmaps.insert("R_walk_up_4", QPixmap(":/Robot/data/character/R_walk_down_4.png"));
    //ui
    framePixmaps.insert("home", QPixmap(":/ui/data/ui/home.png"));
    framePixmaps.insert("pause", QPixmap(":/ui/data/ui/pause.png"));
    framePixmaps.insert("ui", QPixmap(":/ui/data/ui/ui.png"));
    //water ball
    framePixmaps.insert("WB_1", QPixmap(":/WB/data/WB/WB_1.png"));
    framePixmaps.insert("WB_2", QPixmap(":/WB/data/WB/WB_2.png"));
    framePixmaps.insert("WB_3", QPixmap(":/WB/data/WB/WB_3.png"));
    framePixmaps.insert("WB_4", QPixmap(":/WB/data/WB/WB_4.png"));
    framePixmaps.insert("WB_center_1", QPixmap(":/WB/data/WB/WB_center_1.png"));
    framePixmaps.insert("WB_center_2", QPixmap(":/WB/data/WB/WB_center_2.png"));
    framePixmaps.insert("WB_center_3", QPixmap(":/WB/data/WB/WB_center_3.png"));
    framePixmaps.insert("WB_center_4", QPixmap(":/WB/data/WB/WB_4.png"));
    //water ball end
    framePixmaps.insert("WB_end_down_1", QPixmap(":/WB/data/WB/WB_end_down_1.png"));
    framePixmaps.insert("WB_end_down_2", QPixmap(":/WB/data/WB/WB_end_down_2.png"));
    framePixmaps.insert("WB_end_down_3", QPixmap(":/WB/data/WB/WB_end_down_3.png"));
    framePixmaps.insert("WB_end_down_4", QPixmap(":/WB/data/WB/WB_end_down_4.png"));
    framePixmaps.insert("WB_end_left_1", QPixmap(":/WB/data/WB/WB_end_left_1.png"));
    framePixmaps.insert("WB_end_left_2", QPixmap(":/WB/data/WB/WB_end_left_2.png"));
    framePixmaps.insert("WB_end_left_3", QPixmap(":/WB/data/WB/WB_end_left_3.png"));
    framePixmaps.insert("WB_end_left_4", QPixmap(":/WB/data/WB/WB_end_left_4.png"));
    framePixmaps.insert("WB_end_right_1", QPixmap(":/WB/data/WB/WB_end_right_1.png"));
    framePixmaps.insert("WB_end_right_2", QPixmap(":/WB/data/WB/WB_end_right_2.png"));
    framePixmaps.insert("WB_end_right_3", QPixmap(":/WB/data/WB/WB_end_right_3.png"));
    framePixmaps.insert("WB_end_right_4", QPixmap(":/WB/data/WB/WB_end_right_4.png"));
    framePixmaps.insert("WB_end_up_1", QPixmap(":/WB/data/WB/WB_end_up_1.png"));
    framePixmaps.insert("WB_end_up_2", QPixmap(":/WB/data/WB/WB_end_up_2.png"));
    framePixmaps.insert("WB_end_up_3", QPixmap(":/WB/data/WB/WB_end_up_3.png"));
    framePixmaps.insert("WB_end_up_4", QPixmap(":/WB/data/WB/WB_end_up_4.png"));

    framePixmaps.insert("WB_stem_down_1", QPixmap(":/WB/data/WB/WB_stem_down_1.png"));
    framePixmaps.insert("WB_stem_down_2", QPixmap(":/WB/data/WB/WB_stem_down_2.png"));
    framePixmaps.insert("WB_stem_down_3", QPixmap(":/WB/data/WB/WB_stem_down_3.png"));
    framePixmaps.insert("WB_stem_down_4", QPixmap(":/WB/data/WB/WB_stem_down_4.png"));
    framePixmaps.insert("WB_stem_left_1", QPixmap(":/WB/data/WB/WB_stem_left_1.png"));
    framePixmaps.insert("WB_stem_left_2", QPixmap(":/WB/data/WB/WB_stem_left_2.png"));
    framePixmaps.insert("WB_stem_left_3", QPixmap(":/WB/data/WB/WB_stem_left_3.png"));
    framePixmaps.insert("WB_stem_left_4", QPixmap(":/WB/data/WB/WB_stem_left_4.png"));
    framePixmaps.insert("WB_stem_right_1", QPixmap(":/WB/data/WB/WB_stem_right_1.png"));
    framePixmaps.insert("WB_stem_right_2", QPixmap(":/WB/data/WB/WB_stem_right_2.png"));
    framePixmaps.insert("WB_stem_right_3", QPixmap(":/WB/data/WB/WB_stem_right_3.png"));
    framePixmaps.insert("WB_stem_right_4", QPixmap(":/WB/data/WB/WB_stem_right_4.png"));
    framePixmaps.insert("WB_stem_up_1", QPixmap(":/WB/data/WB/WB_stem_up_1.png"));
    framePixmaps.insert("WB_stem_up_2", QPixmap(":/WB/data/WB/WB_stem_up_2.png"));
    framePixmaps.insert("WB_stem_up_3", QPixmap(":/WB/data/WB/WB_stem_up_3.png"));
    framePixmaps.insert("WB_stem_up_4", QPixmap(":/WB/data/WB/WB_stem_up_4.png"));

    // ⚠️ 將所有需要的圖片以 frameName 命名插入完成。
    qDebug() << "SpriteSheetManager: All sprites loaded. Total frames:" << framePixmaps.size();
}

QPixmap SpriteSheetManager::getFrame(const QString &frameName) const {
    if (!framePixmaps.contains(frameName)) {
        qDebug() << "SpriteSheetManager: Frame not found:" << frameName;
        return QPixmap();
    }
    return framePixmaps[frameName];
}
