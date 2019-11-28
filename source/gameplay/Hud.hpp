//--------------------------------------------------------------------------------
// Hud.hpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter and the boss meter
//--------------------------------------------------------------------------------
#pragma once

#include "graphics/ObjectTilePointer.hpp"
#include "graphics/PalettePointer.hpp"

class GameScene;

class Hud final
{
    SinglePalettePointer palPtr;
    ObjectTilePointer iconPtr, bitsPtr;

    GameScene &gameScene();
public:
    Hud() {}
    void init();
    void pushGraphics();
};