//--------------------------------------------------------------------------------
// Player.cpp
//--------------------------------------------------------------------------------
// <insert description here>
//--------------------------------------------------------------------------------
#include "Player.hpp"

#include "GameScene.hpp"
#include "data/sprites/player.hpp"

constexpr int PlayerPriority = 4;

static SinglePaletteAllocator palette EWRAM_BSS(data::sprites::player.png.palette);

constexpr s32f8 PlayerSpeed = 2;

void Player::init(s32f8 x, s32f8 y)
{
    // Copy the player's data to the main palette
    playerPtr = ObjectTilePointer(SpriteSize::s16x16_4bpp);
    playerPtr.setData(data::sprites::player.png.tiles);
    palPtr = SinglePalettePointer(palette);

    pos = vec2(x, y);

    health = maxHealth = 5;
}

void Player::update()
{
    pos.x += PlayerSpeed * key_tri_horz();
    pos.y += PlayerSpeed * key_tri_vert();

    pos.x = std::clamp<s32f8>(pos.x, PlayerWidth/2, SCREEN_WIDTH - PlayerWidth/2);
    pos.y = std::clamp<s32f8>(pos.y, HudSize + PlayerHeight/2, SCREEN_HEIGHT - HudSize - PlayerHeight/2);
}

void Player::pushGraphics()
{
    // Push the sprite
    auto dp = vec2<int>(pos) - PlayerSize/2;
    graphics::oam.pushRegular(dp, SpriteSize::s16x16_4bpp, playerPtr.getTileId(), palPtr.getPalette(), 0, PlayerPriority);
}

void Player::heal(int amount)
{
    if (amount == 0) return;
    if (amount < 0) damage(-amount);
    health += amount;
    if (health > maxHealth)
        health = maxHealth;
}

void Player::damage(int amount)
{
    if (amount == 0) return;
    if (amount < 0) heal(-amount);

    if (invCounter > 0) return;
    if (health > amount)
    {
        health -= amount;
        invCounter = 120;
    }
    else
    {
        // TODO: die
        for (;;); // Just hang up
    }
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