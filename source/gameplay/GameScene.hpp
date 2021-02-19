//--------------------------------------------------------------------------------
// GameScene.hpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a stage
//--------------------------------------------------------------------------------
#pragma once

#include "scenes/IScene.hpp"
#include "Player.hpp"
#include "Stage.hpp"
#include "ui/UserInterface.hpp"
#include "Background.hpp"
#include "util/UnorderedIterableList.hpp"
#include "Enemy.hpp"
#include "Projectiles.hpp"
#include "Explosions.hpp"

constexpr int NumEnemies = 64;
constexpr int UserInterfaceHeight = 16;

class GameScene final : public IScene
{
public:
    GameScene();
    virtual void vblank() override;
    virtual void update() override;
    virtual ~GameScene() {}

    Player player;
    Stage stage;
    UserInterface userInterface;
    Background background;
    Projectiles projectiles;
    Explosions explosions;

    UnorderedIterableList<Enemy, NumEnemies> enemies;
};
