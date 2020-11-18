//--------------------------------------------------------------------------------
// StillImagePointer.hpp
//--------------------------------------------------------------------------------
// Provides a reference-counting pointer to a still image
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include "../graphics/ObjectTilePointer.hpp"
#include "../graphics/SpriteSize.hpp"
#include "../util/AllocatorPointer.hpp"

class StillImageAllocator final : public Allocator<StillImageAllocator>
{
    const TILE* tiles;
    ObjectTilePointer pointer;
    u16 logBlocks;

    void alloc()
    { 
        pointer = ObjectTilePointer(logBlocks);
        pointer.setData(tiles, sizeof(TILE) << logBlocks);
    }

    void clear()
    { 
        pointer = ObjectTilePointer();
    }

public:
    StillImageAllocator(const void* data, u16 logBlocks)
        : tiles((const TILE*)data), logBlocks(logBlocks) {}
    StillImageAllocator(const void* data, SpriteSize size)
        : StillImageAllocator(data, SizeUtils::logBlocks(size)) {}
    ~StillImageAllocator() {}
    auto getTileId() const { return pointer.getTileId(); }

    friend class Allocator<StillImageAllocator>;
    friend class StillImagePointer;
};

class StillImagePointer final : public AllocatorPointer<StillImageAllocator>
{
public:
    INHERIT_ALLOCATOR_CTORS(StillImagePointer, StillImageAllocator);

    auto getTileId() const
    {
        ASSERT(this->allocator);
        return this->allocator->getTileId();
    }
};
