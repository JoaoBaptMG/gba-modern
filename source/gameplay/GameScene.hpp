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
#include "util/UnorderedIterableList.hpp"
#include "Enemy.hpp"
#include "EnemyProjectiles.hpp"
#include "Explosions.hpp"

constexpr int HudSize = 16;
constexpr int NumEnemies = 64;

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
    EnemyProjectiles enemyProjectiles;
    Explosions explosions;

    UnorderedIterableList<Enemy, NumEnemies> enemies;
};
