//--------------------------------------------------------------------------------
// Player.hpp
//--------------------------------------------------------------------------------
// The class for the player
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "Position.hpp"
#include "graphics/ObjectTilePointer.hpp"
#include "graphics/PalettePointer.hpp"

class GameScene;

constexpr int PlayerWidth = 16;
constexpr int PlayerHeight = 32;

class Player final
{
    void listenToCommands();
    GameScene& gameScene();

    ObjectTilePointer playerPtr;
    SinglePalettePointer palPtr;
    u16 health, maxHealth;

public:
    Player() {}
    void init(s32f8 x, s32f8 y);
    void update();
    void pushGraphics();

    u16 getHealth() const { return health; }
    u16 getMaxHealth() const { return maxHealth; }

    Position pos;
    bool inAir;
};
