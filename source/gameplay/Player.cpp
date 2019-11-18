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

void Player::init(s32f8 x, s32f8 y)
{
    tileId = graphics::allocateObjTiles(SpriteSize::_16x32_4bpp);
    graphics::allocateObjPalettes(1, &paletteId);

    // Copy the player's data to the main palette
    memcpy32(&tile_mem_obj[0][tileId], player_png_tiles, sizeof(player_png_tiles) / sizeof(u32));
    memcpy32(&pal_obj_mem[16*paletteId], player_png_palette, sizeof(player_png_palette) / sizeof(u32));

    // Initialize the position
    pos.x = x;
    pos.y = y;

    inAir = false;
}

void Player::update()
{
    pos.vy += Gravity;
    if (pos.vy > 16)
        pos.vy = 16;

    listenToCommands();

    auto result = gameScene().map.movementSimulation(pos, PlayerWidth, PlayerHeight);
    if (result & CollisionResult::Bottom)
    {
        pos.vy = 0;
        inAir = false;
    }
    else if (inAir && (result & CollisionResult::Top)) pos.vy = 0;
}

void Player::listenToCommands()
{
    if (inAir)
    {
        if (key_released(KEY_A) && pos.vy < DecaySpeed)
            pos.vy = DecaySpeed;
    }
    else
    {
        if (key_hit(KEY_A))
        {
            inAir = true;
            pos.vy = JumpSpeed;
        }
    }
    
    if (key_is_down(KEY_LEFT)) pos.vx = -1;
    else if (key_is_down(KEY_RIGHT)) pos.vx = 1;
    else pos.vx = 0;
}

void Player::pushGraphics()
{
    auto dx = s16(pos.x) - gameScene().cameraX;
    auto dy = s16(pos.y) - gameScene().cameraY;

    // Push the sprite, but only if it's not offscreen
    if (dx > -16 && dx < 240 && dy > -32 && dy < 160)
        graphics::pushOAM(
            ATTR0_Y(dy & 255) | ATTR0_REG | ATTR0_4BPP | ATTR0_TALL,
            ATTR1_X(dx & 511) | ATTR1_SIZE_16x32,
            ATTR2_ID(tileId) | ATTR2_PRIO(0) | ATTR2_PALBANK(paletteId));
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
