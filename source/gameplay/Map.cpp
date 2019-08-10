//--------------------------------------------------------------------------------
// Map.cpp
//--------------------------------------------------------------------------------
// Class that handles one of the backgrounds in order to provide a tilemap
//--------------------------------------------------------------------------------
#include "Map.hpp"

#include "graphics.hpp"
#include "GameScene.hpp"

constexpr u16 SB = 31;

// Flags for collision detection and animation
extern u8 tileFlags[NumTiles] IWRAM_DATA;
u8 tileFlags[NumTiles];

void Map::init()
{
    // Configure the background, put it in medium priority, and separate
    // two screen blocks for it
    REG_BG0CNT = BG_PRIO(1) | BG_CBB(0) | BG_4BPP | BG_SBB(SB) | BG_REG_32x32;
}

void Map::loadTileset(const TilesetData* tileset)
{
    // Load the screen and the palette
    memcpy32(tile_mem, tileset->chars, tileset->charDataSize / sizeof(u32));
    memcpy32(pal_bg_mem, tileset->palette, 16 * sizeof(COLOR) * tileset->numPalettes / sizeof(u32));

    // Load the flags to the special IWRAM buffer
    memcpy32(tileFlags, tileset->tileFlags, sizeof(tileFlags) / sizeof(u32));

    // Update the control register
    if (tileset->flags | Flags::Is8bpp) REG_BG0CNT |= BG_8BPP;
    else REG_BG0CNT &= ~BG_8BPP;
}

void Map::loadMap(const MapData& map)
{
    // Load tileset, unless it is the same as before
    if (curMap.tileset != map.tileset)
        loadTileset(map.tileset);

    curMap = map;
}

void Map::loadActors()
{
    // Load actors
    if (curMap.initFunction)
        curMap.initFunction(gameScene());
}

// A single tile
static inline void transferTile(const MapData& map, u16 tx, u16 ty, u16 svx, u16 svy)
{
    const auto& tile = map.tileGraphics(tx, ty);
    se_mem[SB][32*svy + svx] = tile.topLeft;
    se_mem[SB][32*svy + svx+1] = tile.topRight;
    se_mem[SB][32*(svy+1) + svx] = tile.bottomLeft;
    se_mem[SB][32*(svy+1) + svx+1] = tile.bottomRight;
}

// Copy a single fullscreen area of the map
void Map::copyFullScreen()
{
    // Get the actual tile indices
    u16 tx = gameScene().cameraX / 16;
    u16 ty = gameScene().cameraY / 16;

    // Update the "previous" values
    prevtx = tx;
    prevty = ty;

    // Get the screenblock indices
    u16 svx = this->svx = (tx % 16) * 2;
    u16 svy = this->svy = (ty % 16) * 2;

    // Now, transfer the data
    for (u16 j = 0; j < 11; j++)
    {
        // If the screen fits in a single screenblock, do it
        if (svx == 0) for (u16 i = 0; i < 16; i++)
            transferTile(curMap, tx+i, ty, svx+2*i, svy);
        // Else, do it in two runs
        else
        {
            auto size = 16 - svx/2;
            for (u16 i = 0; i < size; i++) // In the first screenblock
                transferTile(curMap, tx+i, ty, svx + 2*i, svy);
            for (u16 i = size; i < 16; i++) // In the second screenblock
                transferTile(curMap, tx+i, ty, svx + 2*i - 32, svy);
        }

        svy += 2;
        svy %= 32;
        ty++;
    }
}

void Map::vblank()
{
    // Transfer the camera values to the background registers
    REG_BG0HOFS = gameScene().cameraX;
    REG_BG0VOFS = gameScene().cameraY;
}

// Schedules to copy a horizontal stripe of the map
static inline void scheduleHorizontal(const MapData& map, u16 tx, u16 ty, u16 svx, u16 svy)
{
    // The maximum number of tiles we can transfer before reaching the edge of the screen
    auto size = 16 - svx/2;
    int dsvx = svx;

    // Transfer the tiles to the bank
    auto horTileBank = (u16*)graphics::newCopyCommand32(&se_mem[SB][32*svy], 32);
    for (u16 i = 0; i < 16; i++)
    {
        // If we reach the edge, go back to the beginning
        if (i == size) dsvx -= 32;

        const auto& tile = map.tileGraphics(tx+i, ty);
        horTileBank[dsvx+2*i] = tile.topLeft;
        horTileBank[dsvx+2*i+1] = tile.topRight;
        horTileBank[dsvx+2*i+32] = tile.bottomLeft;
        horTileBank[dsvx+2*i+33] = tile.bottomRight;
    }
}

// Schedules to copy a vertical stripe of the map
static inline void scheduleVertical(const MapData& map, u16 tx, u16 ty, u16 svx, u16 svy)
{
    // The maximum number of tiles we can transfer before reaching the edge of the screen
    auto size = std::min(11, 16 - svy/2);

    // Transfer the tiles to the bank
    auto vertTileBank = (u16*)graphics::newVerticalCopyCommand32(&se_mem[SB][32*svy + svx], 2*size);
    for (u16 j = 0; j < 11; j++)
    {
        // Pick a new copy command if we reach the edge of the screen 
        if (j == size) vertTileBank = (u16*)graphics::newVerticalCopyCommand32(&se_mem[SB][svx], 22-2*size);

        // Fill in the tiles in the right order, since the vertical command
        // copies two tiles per line
        const auto& tile = map.tileGraphics(tx, ty+j);
        *vertTileBank++ = tile.topLeft;
        *vertTileBank++ = tile.topRight;
        *vertTileBank++ = tile.bottomLeft;
        *vertTileBank++ = tile.bottomRight;
    }
}

void Map::update()
{
    u16 tx = gameScene().cameraX / TileSize;
    u16 ty = gameScene().cameraY / TileSize;

    // Update the screenblock according to the offset positions
    // First in X - using the previous Y to avoid seams
    while (tx > prevtx)
    {
        prevtx++;
        svx = (svx + 2u) & 31;
        scheduleVertical(curMap, prevtx+15, prevty, (svx + 30) & 31, svy);
    }
    while (tx < prevtx)
    {
        prevtx--;
        svx = (svx + 30u) & 31;     // aka (svx-2) & 31, I'm worrying about signedness issues here
        scheduleVertical(curMap, prevtx, prevty, svx, svy);
    }

    // Then in Y - using the new X
    while (ty > prevty)
    {
        prevty++;
        svy = (svy + 2u) & 31;
        scheduleHorizontal(curMap, prevtx, prevty+10, svx, (svy + 20) & 31);
    }
    while (ty < prevty)
    {
        prevty--;
        svy = (svy + 30u) & 31;
        scheduleHorizontal(curMap, prevtx, prevty, svx, svy);
    }
}

GameScene& Map::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *(GameScene*)((std::byte*)this - offsetof(GameScene, map));
#pragma GCC diagnostic pop
}
