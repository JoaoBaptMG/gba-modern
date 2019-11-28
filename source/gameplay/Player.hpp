//--------------------------------------------------------------------------------
// Player.hpp
//--------------------------------------------------------------------------------
// The class for the player
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "graphics/ObjectTilePointer.hpp"
#include "graphics/PalettePointer.hpp"
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

class GameScene;

constexpr int PlayerWidth = 16;
constexpr int PlayerHeight = 32;
constexpr vec2 PlayerSize(PlayerWidth, PlayerHeight);

class Player final
{
    void listenToCommands();
    GameScene& gameScene();

    ObjectTilePointer playerPtr;
    SinglePalettePointer palPtr;
    u16 health, maxHealth, invCounter;

public:
    Player() {}
    void init(s32f8 x, s32f8 y);
    void update();
    void pushGraphics();

    u16 getHealth() const { return health; }
    u16 getMaxHealth() const { return maxHealth; }

    void heal(int amount = 1);
    void damage(int amount = 1);

    vec2<s32f8> pos, vel;
    bool inAir;
};
