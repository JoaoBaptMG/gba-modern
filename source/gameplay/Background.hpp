//--------------------------------------------------------------------------------
// Background.hpp
//--------------------------------------------------------------------------------
// Class for showing a background
//--------------------------------------------------------------------------------
#pragma once

#include "data/BackgroundData.hpp"
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

class GameScene;

class Background final
{
    GameScene& gameScene();
    const BackgroundData* curBackground;

public:
    Background() {}
    void init();
    void load(const BackgroundData& background);
    void vblank();

    vec2<s32f8> offset;
};
