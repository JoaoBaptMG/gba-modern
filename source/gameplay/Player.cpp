//--------------------------------------------------------------------------------
// Player.cpp
//--------------------------------------------------------------------------------
// The class for the player
//--------------------------------------------------------------------------------
#include "Player.hpp"

#include "data/sprites/player.hpp"
#include "graphics.hpp"
#include "GameScene.hpp"

// Don't worry, this is safe
constexpr s32f8 JumpSpeed = -3.5;
constexpr auto DecaySpeed = JumpSpeed / 2;

static SinglePaletteAllocator palette(player_png_palette);

void Player::init(s32f8 x, s32f8 y)
{
    playerPtr = ObjectTilePointer(SpriteSize::_16x32_4bpp);

    // Copy the player's data to the main palette
    playerPtr.setData(player_png_tiles);

    // Allocate to the palette
    palPtr = SinglePalettePointer(palette);

    // Initialize the position
    pos.x = x;
    pos.y = y;

    // Initialize the health
    health = maxHealth = 8;
    health = 5;

    inAir = false;
}

void Player::update()
{
    vel.y += Gravity;
    if (vel.y > 16)
        vel.y = 16;

    listenToCommands();

    auto res = gameScene().map.movementSimulation(pos, vel, PlayerWidth, PlayerHeight);
    pos += vel + res;
    if (res.y < 0)
    {
        vel.y = 0;
        inAir = false;
    }
    else if (inAir && res.y > 0) vel.y = 0;
}

void Player::listenToCommands()
{
    if (inAir)
    {
        if (key_released(KEY_A) && vel.y < DecaySpeed)
            vel.y = DecaySpeed;
    }
    else
    {
        if (key_hit(KEY_A))
        {
            inAir = true;
            vel.y = JumpSpeed;
        }
    }
    
    if (key_is_down(KEY_LEFT)) vel.x = -1;
    else if (key_is_down(KEY_RIGHT)) vel.x = 1;
    else vel.x = 0;
}

void Player::pushGraphics()
{
    auto dp = vec2<s16>(pos) - gameScene().camera;

    // Push the sprite, but only if it's not offscreen
    if (dp.x > -16 && dp.x < 240 && dp.y > -32 && dp.y < 160)
        graphics::oam.pushRegular(dp, SpriteSize::_16x32_4bpp, playerPtr.getTileId(), palPtr.getPalette(), 0);
}

GameScene& Player::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, player));
#pragma GCC diagnostic pop
}
