//--------------------------------------------------------------------------------
// GameScene.hpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#pragma once

#include "scenes/IScene.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "Hud.hpp"
#include "Background.hpp"
#include "util/UnorderedIterableList.hpp"
#include "Enemy.hpp"
#include "Projectiles.hpp"
#include "Explosions.hpp"

constexpr int HudSize = 16;
constexpr int MessageBoxSize = 44;
constexpr int NumEnemies = 64;

class GameScene final : public IScene
{
public:
    GameScene();
    virtual void vblank() override;
    virtual void update() override;
    virtual ~GameScene() {}

    Player player;
    Level level;
    Hud hud;
    Background background;
    Projectiles projectiles;
    Explosions explosions;

    UnorderedIterableList<Enemy, NumEnemies> enemies;
};
