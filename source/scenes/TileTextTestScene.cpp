//--------------------------------------------------------------------------------
// TileTextTestScene.cpp
//--------------------------------------------------------------------------------
// A test scene for text input
//--------------------------------------------------------------------------------
#include "TileTextTestScene.hpp"

#include "data/fonts/monogram_extended.hpp"
#include "text/StringBuilder.hpp"

#if 0
static const char String[] =
    u8"Thìs ís à test òf whát wè câñ\ndó ùsîng thïs téxt êngiñë. We'rè\ngôìng tõ stress ít öút "
    u8"bý\nwrîtïñg ãn éxtêñsivë ämoûnt òf\ntext, só wè cåñ hôpéfüllÿ\nmêasure ìts përfõrmànce.";
#else
static const char String[] =
    u8"This is a test of what we can\ndo using this engine. We're\ngoing to stress it out "
    u8"by\nwriting an extensive amount of\ntext, so we can hopefully\nmeasure its performance.";
#endif

TileTextTestScene::TileTextTestScene() : IScene(), writer(data::fonts::monogram_extended.ttf, &tile_mem[0], 160)
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;

    // Set the display
    REG_BG0CNT = BG_PRIO(1) | BG_CBB(0) | BG_4BPP | BG_SBB(31) | BG_REG_32x32;
    for (u32 y = 0; y < 20; y++)
        for (u32 x = 0; x < 30; x++)
            se_mem[31][32*y + x] = 20*x + y;

    pal_bg_bank[0][1] = CLR_SKYBLUE;
    pal_bg_bank[0][2] = CLR_ORANGE;
    pal_bg_bank[0][3] = CLR_GREEN;

    // Profile the writer
    REG_TM2D = 0; REG_TM3D = 0;
    REG_TM2CNT = 0; REG_TM3CNT = 0;
    REG_TM3CNT = TM_CASCADE | TM_ENABLE;
    REG_TM2CNT = TM_ENABLE;
    writer.write(4, 16, String, 1);

    // Stop the timer
    REG_TM2CNT = 0;
    u32 time = ((u32)REG_TM3D << 16) | REG_TM2D;

    StringBuilder<64> sb;
    sb.append("Time: ", time, " cycles / ", sizeof(String)-1, " bytes.");
    writer.write(4, 156, sb, 2);
}