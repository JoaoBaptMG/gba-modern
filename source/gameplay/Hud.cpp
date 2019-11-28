//--------------------------------------------------------------------------------
// Hud.cpp
//--------------------------------------------------------------------------------
// The class that shows the player's health meter and the boss meter
//--------------------------------------------------------------------------------
#include "Hud.hpp"

#include "graphics.hpp"
#include "GameScene.hpp"
#include "Player.hpp"
#include "data/sprites/hud.hpp"

static SinglePaletteAllocator palette(hud_png_palette);

void Hud::init()
{
    // Allocate the tiles for icon and the bits
    iconPtr = ObjectTilePointer(SpriteSize::s16x16_4bpp);
    bitsPtr = ObjectTilePointer(SpriteSize::s16x16_4bpp);

    // Load the graphics
    iconPtr.setData(hud_png_tiles, sizeof(hud_png_tiles)/2);
    bitsPtr.setData(hud_png_tiles+sizeof(hud_png_tiles)/2, sizeof(hud_png_tiles)/2);

    palPtr = SinglePalettePointer(palette);
}

void Hud::pushGraphics()
{
    // Push the icon
    graphics::oam.pushRegular(vec2(8, 8), SpriteSize::s16x16_4bpp, iconPtr.getTileId(), palPtr.getPalette(), 0);

    const Player& player = gameScene().player;
    int health = player.getHealth(), maxHealth = player.getMaxHealth();
    // Push the health bits
    for (int i = 0; i < maxHealth; i++)
    {
        u16 tileId = bitsPtr.getTileId();
        if (i >= health) tileId++;
        graphics::oam.pushRegular(vec2(24+8*i, 12), SpriteSize::s8x8_4bpp, tileId, palPtr.getPalette(), 0);
    }

    // Push the tip
    graphics::oam.pushRegular(vec2(24+8*maxHealth, 12), SpriteSize::s8x8_4bpp, bitsPtr.getTileId()+2, palPtr.getPalette(), 0);
}

GameScene& Hud::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, hud));
#pragma GCC diagnostic pop
}
