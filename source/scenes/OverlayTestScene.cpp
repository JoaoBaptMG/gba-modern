//--------------------------------------------------------------------------------
// OverlayTestScene.cpp
//--------------------------------------------------------------------------------
// A scene to test the use of overlays
//--------------------------------------------------------------------------------
#include "OverlayTestScene.hpp"

#include "data/fonts/monogram_extended.hpp"
#include "colors.hpp"
#include "text/StringBuilder.hpp"

#include "memory/overlay.hpp"

extern u32 overlayValue0;
extern u32 overlayValue1;

OverlayTestScene::OverlayTestScene() : IScene(), writer(data::fonts::monogram_extended.ttf, vid_mem)
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    writer.write(4, 12, "Overlay Test", colors::DeepSkyBlue);

    if (&overlayValue0 == &overlayValue1)
        writer.write(4, 36, "The addresses are the same", colors::White);
    else writer.write(4, 36, "The addresses are NOT the same", colors::White);

    OVERLAY_LOAD(0);
    {
        StringBuilder<56> sb;
        sb.append("ov0: 0x", hex(overlayValue0), ", ov1: 0x", hex(overlayValue1));
        writer.write(4, 48, sb, colors::White);
    }

    OVERLAY_LOAD(1);
    {
        StringBuilder<56> sb;
        sb.append("ov0: 0x", hex(overlayValue0), ", ov1: 0x", hex(overlayValue1));
        writer.write(4, 60, sb, colors::White);
    }
}