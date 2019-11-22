//--------------------------------------------------------------------------------
// Map.collision.cpp
//--------------------------------------------------------------------------------
// Provides the collision with the tilemap
//--------------------------------------------------------------------------------
#include "Map.hpp"

// The flags are stored in IWRAM to speed the collision routines up, since
// we might have to check 40 or so of those per second
extern u8 tileFlags[NumTiles] IWRAM_DATA;

vec2<s32f8> Map::movementSimulation(vec2<s32f8> pos, vec2<s32f8> vel, s32f8 width, s32f8 height) const
{
    // Update movement in Y
    auto py = pos.y + vel.y;
    auto ny = py;
    if (vel.y != 0)
    {
        if (vel.y > 0)
        {
            // If there is a collision on the bottom side
            if (horizontalStripeCollision(pos.x, py + height, width))
                // Calculate the response vector
                ny = ((py + height) & ~(TileSize-1)) - height;
        }
        else if (vel.y < 0)
        {
            // If there is a collision on the top side
            if (horizontalStripeCollision(pos.x, py, width))
                // Reposition the player so the touch is pushed downards
                ny = (py + TileSize - s32f8::epsilon) & ~(TileSize-1);
        }
    }

    // Update movement in X
    auto px = pos.x + vel.x;
    auto nx = px;
    if (vel.x != 0)
    {
        if (vel.x > 0)
        {
            // If there is a collision on the right side
            if (verticalStripeCollision(nx + width, ny, height))
                // Reposition the player to touch the right side of the tile
                nx = ((px + width) & ~(TileSize-1)) - width;
        }
        else if (vel.x < 0)
        {
            // If there is a collision on the left side  
            if (verticalStripeCollision(nx, ny, height))
                // Reposition the player to touch the left side
                nx = (px + TileSize - s32f8::epsilon) & ~(TileSize-1);
        }
    }

    return vec2(nx - px, ny - py);
}

bool Map::horizontalStripeCollision(s32f8 x, s32f8 y, s32f8 width) const
{
    // Check if there is any collision between the horizontal line
    // that starts at (x,y) and has width, to do this we need to pick the
    // first and last tiles that this line spans and check their attributes
    auto tx1 = s32(x) / TileSize;
    auto tx2 = s32(x + width - s32f8::epsilon) / TileSize;
    auto ty = s32(y) / TileSize;

    for (auto tx = tx1; tx <= tx2; tx++)
        if (tileFlags[curMap.tile(tx, ty)] & TileFlags::Solid)
            return true;
    return false;
}

bool Map::verticalStripeCollision(s32f8 x, s32f8 y, s32f8 height) const
{
    // Check if there is any collision between the vertical line
    // that starts at (x,y) and has height, to do this we need to pick the
    // first and last tiles that this line spans and check their attributes
    auto tx = s32(x) / TileSize;
    auto ty1 = s32(y) / TileSize;
    auto ty2 = s32(y + height - s32f8::epsilon) / TileSize;

    for (auto ty = ty1; ty <= ty2; ty++)
        if (tileFlags[curMap.tile(tx, ty)] & TileFlags::Solid)
            return true;
    return false;
}