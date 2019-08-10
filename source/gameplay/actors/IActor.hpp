//--------------------------------------------------------------------------------
// IActor.hpp
//--------------------------------------------------------------------------------
// Provides a base class for the actors of the game
//--------------------------------------------------------------------------------
#pragma once

class GameScene;

class IActor
{
public:
    IActor() {}
    virtual void update(GameScene& scene) {}
    virtual ~IActor() {}
};

