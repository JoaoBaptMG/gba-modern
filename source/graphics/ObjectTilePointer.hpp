//--------------------------------------------------------------------------------
// ObjectTilePointer.hpp
//--------------------------------------------------------------------------------
// RAII wrapper around the object allocators for graphics
//--------------------------------------------------------------------------------

#pragma once

#include "graphics.hpp"
#include <algorithm>

class ObjectTilePointer
{
    // The tile index
    u16 tileId;

public:
    // An empty pointer
    ObjectTilePointer() : tileId(graphics::NoTile) {}

    // Allocates a new block
    ObjectTilePointer(SpriteSize size)
        : tileId(graphics::allocateObjTiles(size)) {}

    // Frees the block with it
    ~ObjectTilePointer()
    {
        graphics::freeObjTiles(tileId);
    }

    // Disable copying
    ObjectTilePointer(const ObjectTilePointer& other) = delete;
    ObjectTilePointer& operator=(const ObjectTilePointer& other) = delete;

    // Move constructor
    ObjectTilePointer(ObjectTilePointer&& o) : tileId(o.tileId)
    {
        o.tileId = graphics::NoTile;
    }

    // Move operator
    ObjectTilePointer& operator=(ObjectTilePointer&& o)
    {
        std::swap(tileId, o.tileId);
        return *this;
    }

    // gets the tile ID
    u16 getTileId() const { return tileId; }
};
