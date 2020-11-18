//--------------------------------------------------------------------------------
// IScene.hpp
//--------------------------------------------------------------------------------
// The base class for scenes in the game
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include <type_traits>
#include "../util/ObjectStack.hpp"

// The base class for a scene
class IScene
{
public:
    IScene() {}
    virtual void update() {}
    virtual void vblank() {}
    virtual ~IScene() {}
};

// The scene stack
using SceneStack = ObjectStack<IScene, 65536, 4>;
extern SceneStack sceneStack EWRAM_BSS;
extern bool popScene EWRAM_BSS;

