//--------------------------------------------------------------------------------
// Map.cpp
//--------------------------------------------------------------------------------
// Class that handles one of the backgrounds in order to provide a tilemap
//--------------------------------------------------------------------------------
#include "Map.hpp"

#include "graphics.hpp"
#include "GameScene.hpp"

// constexpr utilities
constexpr uint SB = 31;
constexpr uint ScreenBlockTiles = 32;
constexpr SCR_ENTRY* ScreenLocation(int screenDestX, int screenDestY)
{
    return &se_mem[SB][ScreenBlockTiles * screenDestY + screenDestX];
}

// Flags for collision detection and animation
u8 tileFlags[NumTiles] IWRAM_DATA;

void Map::init()
{
    // Configure the background, put it in medium prioritileY, and separate
    // one screen block for it
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
    if (tileset->flags & Flags::Is8bpp) REG_BG0CNT |= BG_8BPP;
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
static inline void transferTile(const MapData& map, u16 tileX, u16 tileY, u16 screenDestX, u16 screenDestY)
{
    const Tile& tile = map.tileGraphics(tileX, tileY);
    *ScreenLocation(screenDestX, screenDestY) = tile.topLeft;
    *ScreenLocation(screenDestX + 1, screenDestY) = tile.topRight;
    *ScreenLocation(screenDestX, screenDestY + 1) = tile.bottomLeft;
    *ScreenLocation(screenDestX + 1, screenDestY + 1) = tile.bottomRight;
}

// Copy a single fullscreen area of the map
void Map::copyFullScreen()
{
    // Get the actual tile indices
    uint tileX = gameScene().cameraX / TileSize;
    uint tileY = gameScene().cameraY / TileSize;

    // Update the "previous" values
    prevTileX = tileX;
    prevTileY = tileY;

    // Get the screenblock indices
    uint screenDestX = (2 * tileX) % ScreenBlockTiles;
    uint screenDestY = (2 * tileY) % ScreenBlockTiles;

    // Now, transfer the data
    for (u16 j = 0; j < 11; j++)
    {
        // If the screen fits in a single screenblock, do it
        if (screenDestX == 0) for (u16 i = 0; i < 16; i++)
            transferTile(curMap, tileX+i, tileY, screenDestX+2*i, screenDestY);
        // Else, do it in two runs
        else
        {
            auto size = 16 - screenDestX/2;
            for (u16 i = 0; i < size; i++) // In the first screenblock
                transferTile(curMap, tileX+i, tileY, screenDestX + 2*i, screenDestY);
            for (u16 i = size; i < 16; i++) // In the second screenblock
                transferTile(curMap, tileX+i, tileY, screenDestX + 2*i - ScreenBlockTiles, screenDestY);
        }

        screenDestY += 2;
        screenDestY %= ScreenBlockTiles;
        tileY++;
    }
}

void Map::vblank()
{
    // Transfer the camera values to the background registers
    REG_BG0HOFS = gameScene().cameraX;
    REG_BG0VOFS = gameScene().cameraY;
}

// Schedules to copy a horizontal stripe of the map
static inline void scheduleHorizontal(const MapData& map, u16 tileX, u16 tileY)
{
    uint screenDestX = (2 * tileX) % ScreenBlockTiles;
    uint screenDestY = (2 * tileY) % ScreenBlockTiles;
    constexpr uint NumTiles = SCREEN_WIDTH / TileSize + 1;

    // The maximum number of tiles we can transfer before reaching the edge of the screen
    uint size = NumTiles - screenDestX / 2;

    // Transfer the tiles to the bank
    u16* command = (u16*)graphics::newCopyCommand32(ScreenLocation(0, screenDestY), 32);
    for (uint i = 0; i < NumTiles; i++)
    {
        // If we reach the edge, go back to the beginning
        if (i == size) screenDestX -= ScreenBlockTiles;

        const Tile& tile = map.tileGraphics(tileX+i, tileY);
        command[screenDestX + 2*i] = tile.topLeft;
        command[screenDestX + 2*i+1] = tile.topRight;
        command[screenDestX + 2*i+32] = tile.bottomLeft;
        command[screenDestX + 2*i+33] = tile.bottomRight;
    }
}

// Schedules to copy a vertical stripe of the map
static inline void scheduleVertical(const MapData &map, u16 tileX, u16 tileY)
{
    // Compute the destination of the tiles
    uint screenDestX = (2 * tileX) % ScreenBlockTiles;
    uint screenDestY = (2 * tileY) % ScreenBlockTiles;
    constexpr uint NumTiles = SCREEN_HEIGHT / TileSize + 1;

    // The maximum number of tiles we can transfer before reaching the edge of the screen
    uint size = std::min(NumTiles, (ScreenBlockTiles - screenDestY) / (uint)2);

    // Transfer the tiles to the bank
    auto command = (u16*)graphics::newVerticalCopyCommand32(ScreenLocation(screenDestX, screenDestY), 2*size);
    for (uint j = 0; j < NumTiles; j++)
    {
        // Pick a new copy command if we reach the edge of the screen 
        if (j == size) command = (u16*)graphics::newVerticalCopyCommand32(ScreenLocation(screenDestX, 0), 2*(NumTiles-size));

        // Fill in the tiles in the right order, since the vertical command
        // copies two tiles per line
        const Tile& tile = map.tileGraphics(tileX, tileY + j);
        command[0] = tile.topLeft;
        command[1] = tile.topRight;
        command[2] = tile.bottomLeft;
        command[3] = tile.bottomRight;
        command += 4;
    }
}

void Map::update()
{
    uint tileX = gameScene().cameraX / TileSize;
    uint tileY = gameScene().cameraY / TileSize;

    // Update the screenblock according to the offset positions
    // First in X - using the previous Y to avoid seams
    while (tileX > prevTileX)
    {
        prevTileX++;
        scheduleVertical(curMap, prevTileX+15, prevTileY);
    }
    while (tileX < prevTileX)
    {
        prevTileX--;
        scheduleVertical(curMap, prevTileX, prevTileY);
    }

    // Then in Y - using the new X
    while (tileY > prevTileY)
    {
        prevTileY++;
        scheduleHorizontal(curMap, prevTileX, prevTileY+10);
    }
    while (tileY < prevTileY)
    {
        prevTileY--;
        scheduleHorizontal(curMap, prevTileX, prevTileY);
    }
}

GameScene& Map::gameScene()
{
    // Don't worry, I know what I'm doing
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
    return *reinterpret_cast<GameScene*>(
        reinterpret_cast<std::byte*>(this) - offsetof(GameScene, map));
#pragma GCC diagnostic pop
}
