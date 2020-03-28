//--------------------------------------------------------------------------------
// PlayerProjectiles.cpp
//--------------------------------------------------------------------------------
// The class that manages the player projectiles
//--------------------------------------------------------------------------------
#include "PlayerProjectiles.hpp"

#include "GameScene.hpp"
#include <algorithm>

#include "data/sprites/player-projectiles.hpp"

constexpr int PlayerProjectilePriority = 6;

static SinglePaletteAllocator palette EWRAM_BSS(data::sprites::player_projectiles.png.palette);

void PlayerProjectiles::init()
{
    // Set everything to zero
    memset32(projectiles, 0, sizeof(projectiles)/sizeof(u32));
    numProjectiles = 0;

    // Initialize the graphics pointers
    tilePtr = ObjectTilePointer(SpriteSize::s8x8_4bpp);
    tilePtr.setData(data::sprites::player_projectiles.png.tiles);

    palPtr = SinglePalettePointer(palette);
}

void PlayerProjectiles::update()
{
    // Update each position and check if it's offscreen
    for (u32 i = 0; i < numProjectiles;)
    {
        projectiles[i].pos += projectiles[i].vel;

        // If it's offscreen, swap with a final projectile
        auto pos = projectiles[i].pos;
        if (pos.x < -2 || pos.x > SCREEN_WIDTH + 2 || pos.y < -2 || pos.y > SCREEN_HEIGHT + 2)
            std::swap(projectiles[i], projectiles[--numProjectiles]);
        else i++;
    }
}

void PlayerProjectiles::pushGraphics()
{
    // Add a sprite for each projectile on screen
    for (u32 i = 0; i < numProjectiles; i++)
    {
        auto dp = vec2<int>(projectiles[i].pos) - vec2(4, 4);
        graphics::oam.pushRegular(dp, SpriteSize::s8x8_4bpp, tilePtr.getTileId(),
            palPtr.getPalette(), 0, PlayerProjectilePriority);
    }
}

GameScene& PlayerProjectiles::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, playerProjectiles));
#pragma GCC diagnostic pop
}