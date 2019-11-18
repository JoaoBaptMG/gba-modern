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
    iconPtr = ObjectTilePointer(SpriteSize::_16x16_4bpp);
    bitsPtr = ObjectTilePointer(SpriteSize::_16x16_4bpp);

    // Load the graphics
    iconPtr.setData(hud_png_tiles, sizeof(hud_png_tiles)/2);
    bitsPtr.setData(hud_png_tiles+sizeof(hud_png_tiles)/2, sizeof(hud_png_tiles)/2);

    palPtr = SinglePalettePointer(palette);
}

void Hud::pushGraphics()
{
    // Push the icon
    graphics::pushOAM(
        ATTR0_Y(8) | ATTR0_REG | ATTR0_4BPP | ATTR0_REG,
        ATTR1_X(8) | ATTR1_SIZE_16x16,
        ATTR2_ID(iconPtr.getTileId()) | ATTR2_PRIO(0) | ATTR2_PALBANK(palPtr.getPalette()));

    const Player& player = gameScene().player;
    int health = player.getHealth(), maxHealth = player.getMaxHealth();
    // Push the health bits
    for (int i = 0; i < maxHealth; i++)
    {
        u16 tileId = bitsPtr.getTileId();
        if (i >= health) tileId++;
        graphics::pushOAM(
            ATTR0_Y(12) | ATTR0_REG | ATTR0_4BPP | ATTR0_REG,
            ATTR1_X(24+8*i) | ATTR1_SIZE_8x8,
            ATTR2_ID(tileId) | ATTR2_PRIO(0) | ATTR2_PALBANK(palPtr.getPalette()));
    }

    // Push the tip
    graphics::pushOAM(
        ATTR0_Y(12) | ATTR0_REG | ATTR0_4BPP | ATTR0_REG,
        ATTR1_X(24 + 8*maxHealth) | ATTR1_SIZE_8x8,
        ATTR2_ID(bitsPtr.getTileId()+2) | ATTR2_PRIO(0) | ATTR2_PALBANK(palPtr.getPalette()));
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
