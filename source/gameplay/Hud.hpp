//--------------------------------------------------------------------------------
// Hud.hpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter, score and boss meter
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

class GameScene;

class Hud final
{
    GameScene &gameScene();

    u16 clearAlpha, reloadAlpha;

public:
    Hud() {}
    void init();
    void vblank();
    void update();
};
