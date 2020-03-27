//--------------------------------------------------------------------------------
// Player.hpp
//--------------------------------------------------------------------------------
// The class for the player
//--------------------------------------------------------------------------------
#pragma once

#include "graphics/graphics.hpp"
#include "graphics/ObjectTilePointer.hpp"
#include "graphics/PalettePointer.hpp"
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

class GameScene;

constexpr int PlayerWidth = 16;
constexpr int PlayerHeight = 16;
constexpr vec2 PlayerSize(PlayerWidth, PlayerHeight);

class Player final
{
    GameScene& gameScene();

    ObjectTilePointer playerPtr;
    SinglePalettePointer palPtr;

public:
    vec2<s32f8> pos;

    Player() {}
    void init(s32f8 x, s32f8 y);
    void update();
    void pushGraphics();
};
