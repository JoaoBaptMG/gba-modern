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