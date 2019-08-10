//--------------------------------------------------------------------------------
// Map.collision.cpp
//--------------------------------------------------------------------------------
// Provides the collision with the tilemap
//--------------------------------------------------------------------------------
#include "Map.hpp"

// The flags are stored in IWRAM to speed the collision routines up, since
// we might have to check 40 or so of those per second
extern u8 tileFlags[NumTiles] IWRAM_DATA;

CollisionResult Map::movementSimulation(Position& pos, s32f8 width, s32f8 height) const
{
    CollisionResult res = CollisionResult::None;

    // Update movement in Y
    if (pos.vy != 0)
    {
        pos.y += pos.vy;

        if (pos.vy > 0)
        {
            // If there is a collision on the bottom side
            if (horizontalStripeCollision(pos.x, pos.y+height, width))
            {
                // Reposition the player so the bottom is pushed upwards
                pos.y = ((pos.y + height) & ~(TileSize-1)) - height;
                res |= CollisionResult::Bottom;
            }
        }
        else if (pos.vy < 0)
        {
            // If there is a collision on the top side
            if (horizontalStripeCollision(pos.x, pos.y, width))
            {
                // Reposition the player so the touch is pushed downards
                pos.y = (pos.y + TileSize - s32f8::epsilon) & ~(TileSize-1);
                res |= CollisionResult::Top;
            }
        }
    }

    // Update movement in X
    if (pos.vx != 0)
    {
        pos.x += pos.vx;

        if (pos.vx > 0)
        {
            // If there is a collision on the right side
            if (verticalStripeCollision(pos.x+width, pos.y, height))
            {
                // Reposition the player to touch the right side of the tile
                pos.x = ((pos.x + width) & ~(TileSize-1)) - width;
                res |= CollisionResult::Right;
            }
        }
        else if (pos.vx < 0)
        {
            // If there is a collision on the left side  
            if (verticalStripeCollision(pos.x, pos.y, height))
            {
                // Reposition the player to touch the left side
                pos.x = (pos.x + TileSize - s32f8::epsilon) & ~(TileSize-1);
                res |= CollisionResult::Left;
            }
        }
    }

    return res;
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