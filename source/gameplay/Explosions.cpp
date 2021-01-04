//--------------------------------------------------------------------------------
// Explosions.cpp
//--------------------------------------------------------------------------------
// Manage the explosions in the game
//--------------------------------------------------------------------------------
#include "Explosions.hpp"

#include "data/sprites/small-explosion.hpp"

constexpr auto ExplosionPriority = 16;
constexpr u32 FramesPerFrame = 4; // GBA frames / animation frame

static SinglePaletteAllocator smallExplosionPalette EWRAM_BSS(data::sprites::small_explosion.png.palette);

void Explosions::init()
{
    // Initialize all the graphic datas
    constexpr auto Tile16x16size = 4 * sizeof(TILE4);
    for (u32 i = 0; i < NumSmallExplosionFrames; i++)
    {
        smallExplosion[i] = ObjectTilePointer(SpriteSize::s16x16_4bpp);
        smallExplosion[i].setData(data::sprites::small_explosion.png.tiles + i * Tile16x16size, Tile16x16size);
    }

    smallPtr = SinglePalettePointer(smallExplosionPalette);
    numExplosions = 0;
}

void Explosions::update()
{
    // Update the explosions
    for (u32 i = 0; i < numExplosions;)
    {
        // Update the explosion counter
        explosions[i].counter++;
        if (explosions[i].counter == NumSmallExplosionFrames * FramesPerFrame)
        {
            explosions[i] = explosions[--numExplosions];
            explosionHandles[numExplosions].obj = UniqueOamHandle::noObj();
        }
        else i++;
    }
}

void Explosions::updateGraphics()
{
    // Push the explosion graphics
    for (u32 i = 0; i < numExplosions; i++)
    {
        auto pos = vec2<int>(explosions[i].pos) - vec2(8, 8);
        auto frame = explosions[i].counter / FramesPerFrame;
        explosionHandles[i].obj.setRegular(pos, SpriteSize::s16x16_4bpp,
            smallExplosion[frame].getTileId(),
            smallPtr.getPalette(), 1, ExplosionPriority);
    }
}