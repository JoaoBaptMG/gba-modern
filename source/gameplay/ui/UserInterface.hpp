//--------------------------------------------------------------------------------
// UserInterface.hpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter, score and boss meter
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include <variant>

#include "text/Tile4bppGlyphWriter.hpp"
#include "text/TextWriter.hpp"

#include "MsgBox.hpp"
#include "LevelSign.hpp"

class GameScene;

class UserInterface final
{
    GameScene &gameScene();

    TextWriter<Tile4bppGlyphWriter> scoreWriter;
    u16 clearAlpha, reloadAlpha;

    using Signs = std::variant<std::monostate, LevelSign>;
    Signs sign;

public:
    UserInterface();
    void init();
    void vblank();
    void update();

    MsgBox msgBox;

    void displayLevel(int level) { sign.emplace<LevelSign>(level); }
};
