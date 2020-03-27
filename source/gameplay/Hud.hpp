//--------------------------------------------------------------------------------
// Hud.hpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter, score and boss meter
//--------------------------------------------------------------------------------
#pragma once

class GameScene;

class Hud final
{
    GameScene &gameScene();

public:
    Hud() {}
    void init();
    void vblank();
    void update();
};
