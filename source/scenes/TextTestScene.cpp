//--------------------------------------------------------------------------------
// TextTestScene.cpp
//--------------------------------------------------------------------------------
// A test scene for text input
//--------------------------------------------------------------------------------
#include "TextTestScene.hpp"

#include "data/fonts/PixelFJVerdana12pt.hpp"

TextTestScene::TextTestScene() : IScene(), writer(data::fonts::PixelFJVerdana12pt)
{
    // Set the display registers
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    // Test the writer
    writer.write(4, 80, "This is a test!\nWE CAN JUMP LINES!", CLR_SKYBLUE);
}