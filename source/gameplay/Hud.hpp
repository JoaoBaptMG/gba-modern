//--------------------------------------------------------------------------------
// Hud.hpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter, score and boss meter
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

#include "text/Tile4bppGlyphWriter.hpp"
#include "text/TextWriter.hpp"

class GameScene;

class Hud final
{
    GameScene &gameScene();

    TextWriter<Tile4bppGlyphWriter> scoreWriter;
    u16 clearAlpha, reloadAlpha;

public:
    Hud();
    void init();
    void vblank();
    void update();
};
