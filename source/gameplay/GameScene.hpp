//--------------------------------------------------------------------------------
// GameScene.hpp
//--------------------------------------------------------------------------------
// The main scene of the game - provides the necessary plumbing to display a level
//--------------------------------------------------------------------------------
#pragma once

#include "scenes/IScene.hpp"
#include "util/UnorderedPolymorphicList.hpp"
#include "util/LightBitset.hpp"
#include "util/WrappedContainer.hpp"
#include "actors/IActor.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "Hud.hpp"
#include "math/vec2.hpp"

constexpr u32 NumActors = 64;
constexpr u32 ActorSize = 64;

class GameScene final : public IScene
{
    vec2<s16> calculateCameraVector() const;
    LightBitset<NumActors> actorsToRemove;
    UnorderedPolymorphicList<IActor, ActorSize, NumActors> actors;

public:
    GameScene();
    virtual void vblank() override;
    virtual void update() override;
    virtual ~GameScene() {}

    template <typename T, typename... Ts>
    void addActor(Ts&&... ts) { actors.add<T>(std::forward<Ts>(ts)...); }
    void removeActor(IActor& actor) { ASSERT(true); actorsToRemove.set(actors.indexOf(&actor)); }
    auto getActors() { return WrappedContainer(actors); }

    Map map;
    Player player;
    Hud hud;
    vec2<s16> camera;
};
