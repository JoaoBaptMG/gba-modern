//--------------------------------------------------------------------------------
// Player.hpp
//--------------------------------------------------------------------------------
// The class for the player
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "graphics/ObjectTilePointer.hpp"
#include "graphics/StreamAnimator.hpp"
#include "graphics/PalettePointer.hpp"
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

class GameScene;

constexpr int PlayerWidth = 16;
constexpr int PlayerHeight = 32;
constexpr int MeleeAttackWidth = 24;
constexpr int MeleeAttackHeight = 32;
constexpr vec2 PlayerSize(PlayerWidth, PlayerHeight);

class Player final
{
    void listenToCommands();
    GameScene& gameScene();

    ObjectTilePointer playerPtr, meleePtr;
    SinglePalettePointer palPtr;
    StreamAnimator meleeAnimator;
    u16 health, maxHealth, invCounter;
    u16 money;

    // flags
    u16 goingLeft:1;

public:
    vec2<s32f8> pos, vel;
    bool inAir;

    Player();
    void init(s32f8 x, s32f8 y);
    void update();
    void pushGraphics();

    u16 getHealth() const { return health; }
    u16 getMaxHealth() const { return maxHealth; }

    void heal(int amount = 1);
    void damage(int amount = 1);

    void giveMoney(int amount = 1);

    void respondToGravityAndCollision();
    void triggerMelee();
    void observeMelee();

    // Used by the actors
    auto getMeleePos() const { return pos + vec2(goingLeft ? -MeleeAttackWidth : PlayerWidth, 0); }
    auto getMeleeSize() { return vec2<s32f8>(MeleeAttackWidth, MeleeAttackHeight); }
};
