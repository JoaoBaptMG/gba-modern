//--------------------------------------------------------------------------------
// Money.cpp
//--------------------------------------------------------------------------------
// A single piece of money
//--------------------------------------------------------------------------------
#include "Money.hpp"

#include "gameplay/GameScene.hpp"
#include "gameplay/collision.hpp"

#include "data/sprites/money.hpp"

using namespace data::sprites;

constexpr auto MoneySide = 8;
constexpr auto MoneySize = vec2(MoneySide, MoneySide);

constexpr double Period = 120;
constexpr double Amplitude = 8;
constexpr double Pi = 3.14159265358979323846;

constexpr s32f16 AccelCounter = (2 * Pi / Period) * (2 * Pi / Period);
constexpr s32f8 InitialVelocity = Pi * Amplitude / Period;

static StillImageAllocator tiles EWRAM_BSS (money.png.tiles, SpriteSize::s8x8_4bpp);
static SinglePaletteAllocator palette EWRAM_BSS (money.png.palette);

Money::Money(s32f8 x, s32f8 y) : tilePtr(tiles, false), palPtr(palette, false)
{
    pos = vec2(x, y);
    eqy = pos.y;
    size = MoneySize;
    yvel = -InitialVelocity;
}

void Money::update(GameScene& scene)
{
    // Move acording to the spring equation
    pos.y += yvel;
    yvel += AccelCounter * (eqy - pos.y);

    // If collected by the player, give him money
    auto& player = scene.player;
    if (collision::box<>(pos, MoneySize, player.pos, PlayerSize))
    {
        player.giveMoney();
        scene.removeActor(*this);
    }

    // Push graphics
    pushGraphics(scene.camera);
}

bool Money::updateVisibility(bool visible)
{
    // Set the visibilities
    tilePtr.setActive(visible);
    palPtr.setActive(visible);
    return visible;
}

void Money::pushGraphics(vec2<s16> camera)
{
    auto dp = vec2<int>(pos) - camera;

    // Set the animator's visibility
    bool visible = updateVisibility(dp.x > -8 && dp.x < 240 && dp.y > -8 && dp.y < 160);

    // Push the sprite, but only if it's not offscreen
    if (visible) graphics::oam.pushRegular(dp, SpriteSize::s16x16_4bpp, tilePtr.getTileId(), palPtr.getPalette(), 0, 4);
}