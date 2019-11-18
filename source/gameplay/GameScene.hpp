//--------------------------------------------------------------------------------
// GameScene.hpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#pragma once

#include "scenes/IScene.hpp"
#include "util/PointerInterfaceContainer.hpp"
#include "actors/IActor.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "Hud.hpp"

constexpr u32 NumActors = 64;
constexpr u32 ActorSize = 64;

class GameScene final : public IScene
{
    s16 calculateCameraX() const;
    s16 calculateCameraY() const;

public:
    GameScene();
    virtual void vblank() override;
    virtual void update() override;
    virtual ~GameScene() {}

    Map map;
    Player player;
    Hud hud;
    u16 cameraX, cameraY;
    PointerInterfaceContainer<IActor, ActorSize> actors[NumActors];
};
