//--------------------------------------------------------------------------------
// Mode3TextTestScene.cpp
//--------------------------------------------------------------------------------
// A test scene for text input
//--------------------------------------------------------------------------------
#include "Mode3TextTestScene.hpp"

#include "data/fonts/Anonymous.hpp"
#include "text/StringBuilder.hpp"

Mode3TextTestScene::Mode3TextTestScene() : IScene(), writer(data::fonts::Anonymous, vid_mem)
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    // Profile the writer
    REG_TM2D = 0; REG_TM3D = 0;
    REG_TM2CNT = 0; REG_TM3CNT = 0;
    REG_TM3CNT = TM_CASCADE | TM_ENABLE;
    REG_TM2CNT = TM_ENABLE;
    writer.write(4, 16, "This is a test of what we can\ndo using this engine. We're\ngoing to stress it out "
        "by\nwriting an extensive amount of\ntext, so we can hopefully\nmeasure its performance.", CLR_SKYBLUE);

    // Stop the timer
    REG_TM2CNT = 0;
    u32 time = ((u32)REG_TM3D << 16) | REG_TM2D;

    StringBuilder<64> sb;
    sb.append("Time: ", time, " cycles.");
    writer.write(4, 156, sb, CLR_ORANGE);
}