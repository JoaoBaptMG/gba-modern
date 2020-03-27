//--------------------------------------------------------------------------------
// GameScene.hpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#pragma once

#include "scenes/IScene.hpp"
#include "Player.hpp"
#include "Hud.hpp"

constexpr int HudSize = 24;

class GameScene final : public IScene
{
public:
    GameScene();
    virtual void vblank() override;
    virtual void update() override;
    virtual ~GameScene() {}

    Player player;
    Hud hud;
};
