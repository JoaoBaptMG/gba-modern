//--------------------------------------------------------------------------------
// DivTestScene.cpp
//--------------------------------------------------------------------------------
// A scene to test signed division results
//--------------------------------------------------------------------------------
#include "DivTestScene.hpp"

#include "data/fonts/monogram_extended.hpp"
#include "colors.hpp"
#include "text/StringBuilder.hpp"

DivTestScene::DivTestScene() : IScene(), writer(vid_mem)
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    const auto& font = data::fonts::monogram_extended.ttf;

    writer.write(4, 12, "Signed Division Test", colors::DeepSkyBlue, font);

    StringBuilder<128> sb;

    auto divTest = [&sb](int a, int b)
    {
        sb.append(a, " DIV ", b, " = ", a/b, " mod ", a%b, '\n');
    };

    divTest(37, 5);
    divTest(37, -5);
    divTest(-37, 5);
    divTest(-37, -5);

    writer.write(4, 36, sb, colors::White, font);
}
