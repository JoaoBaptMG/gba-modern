//--------------------------------------------------------------------------------
// GameScene.hpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#pragma once

#include "scenes/IScene.hpp"
#include "Player.hpp"
#include "PlayerProjectiles.hpp"
#include "Level.hpp"
#include "Hud.hpp"
#include "Background.hpp"
#include "util/UnorderedList.hpp"
#include "Enemy.hpp"

constexpr int HudSize = 16;

class GameScene final : public IScene
{
public:
    GameScene();
    virtual void vblank() override;
    virtual void update() override;
    virtual ~GameScene() {}

    Player player;
    PlayerProjectiles playerProjectiles;
    Level level;
    Hud hud;
    Background background;

    UnorderedList<Enemy, 64> enemies;
    inline void addEnemy(EnemyScript script) { enemies.add(script); }
};
