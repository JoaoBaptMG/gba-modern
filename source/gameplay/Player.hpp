//--------------------------------------------------------------------------------
// Player.hpp
//--------------------------------------------------------------------------------
// The class for the player
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "Position.hpp"

class GameScene;

constexpr int PlayerWidth = 16;
constexpr int PlayerHeight = 32;

class Player final
{
    void listenToCommands();
    GameScene& gameScene();

    u32 tileId, paletteId;

public:
    Player() {}
    void init(s32f8 x, s32f8 y);
    void update();
    void pushGraphics();

    Position pos;
    bool inAir;
};
