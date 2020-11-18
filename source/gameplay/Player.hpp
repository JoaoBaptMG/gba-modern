//--------------------------------------------------------------------------------
// Player.hpp
//--------------------------------------------------------------------------------
// The class for the player
//--------------------------------------------------------------------------------
#pragma once

#include "../graphics/graphics.hpp"
#include "../graphics/ObjectTilePointer.hpp"
#include "../graphics/PalettePointer.hpp"
#include "../math/stdfixed.hpp"
#include "../math/vec2.hpp"

class GameScene;

constexpr int PlayerWidth = 16;
constexpr int PlayerHeight = 16;
constexpr vec2 PlayerSize(PlayerWidth, PlayerHeight);

constexpr int PlayerTargetRadius = 3;

class Player final
{
    GameScene& gameScene();

    ObjectTilePointer playerPtr;
    SinglePalettePointer palPtr;
    u16 health, maxHealth, invCounter;
    u16 shootCooldown;

public:
    vec2<s32f16> pos;

    Player() {}
    void init(s32f16 x, s32f16 y);
    void update();
    void pushGraphics();

    u16 getHealth() const { return health; }
    u16 getMaxHealth() const { return maxHealth; }

    void heal(int amount = 1);
    void damage(int amount = 1);
};
