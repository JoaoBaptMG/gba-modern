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
    ObjectTilePointer iconPtr, bitsPtr, numberPtrs[2];
    u16 money, moneyDigits;
    s16 moneyDisplayCounter;

    void createNewDigits();
    void displayMoney();
    GameScene &gameScene();

public:
    Hud() {}
    void init();
    void pushGraphics();

    void notifyMoneyChange(u16 newMoney);
};