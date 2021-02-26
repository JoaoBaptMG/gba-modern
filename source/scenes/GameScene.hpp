//--------------------------------------------------------------------------------
// GameScene.hpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a stage
//--------------------------------------------------------------------------------
#pragma once

#include "IScene.hpp"

#include "gameplay/Player.hpp"
#include "gameplay/Stage.hpp"
#include "gameplay/ui/UserInterface.hpp"
#include "gameplay/Background.hpp"
#include "gameplay/Enemy.hpp"
#include "gameplay/Projectiles.hpp"
#include "gameplay/Explosions.hpp"
#include "util/UnorderedIterableList.hpp"

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
