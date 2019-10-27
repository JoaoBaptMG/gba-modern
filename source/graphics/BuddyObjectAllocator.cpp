//--------------------------------------------------------------------------------
// BuddyObjectAllocator.cpp
//--------------------------------------------------------------------------------
// Provide a tile memory allocator for the VRAM, for the object tiles
//--------------------------------------------------------------------------------

#include "BuddyObjectAllocator.hpp"
#include "util/gba-assert.hpp"
#include "graphics.hpp"

// For debugging purposes
#define CLEAR_TILE_AFTER

// In use flag
constexpr u16 InUse = 0x8000;

BuddyObjectAllocator::BuddyObjectAllocator()
{
    // Populate all the free list - UB, I know, but I don't care
    constexpr uint Size = sizeof(blocks) + sizeof(freeList);
    memset32(blocks, dup16(graphics::NoTile), Size / sizeof(u32));

    // Set the first block's memory
    freeList[LogNumTiles] = 0;
}

// Allocate a block with a tile order
u16 BuddyObjectAllocator::allocBlock(SpriteSize size)
{
    uint order = (uint)size;

    // Find the same (or next superior) order block
    uint foundOrder;
    for (foundOrder = order; foundOrder <= LogNumTiles; foundOrder++)
        if (freeList[foundOrder] != graphics::NoTile) break;

    // Now, if we found a block, split if it required
    if (foundOrder <= LogNumTiles)
    {
        // Take the block away from the free list, and update the links
        u16 tile = freeList[foundOrder];
        freeList[foundOrder] = blocks[tile].next;
        if (blocks[tile].next != graphics::NoTile)
            blocks[blocks[tile].next].prev = graphics::NoTile;

        // If the order was a superior one, split the block
        while (foundOrder > order)
        {
            // Descend to the previous order
            foundOrder--;

            // Create the buddy block and puts it into the free list
            u16 buddy = tile + (1 << foundOrder);
            u16 first = freeList[foundOrder];
            if (first != graphics::NoTile)
                blocks[first].prev = buddy;
            blocks[buddy].next = first;
            blocks[buddy].prev = graphics::NoTile;
            freeList[foundOrder] = buddy;

            // Now we have a block at tile, but with a order lowererd by one
        }

        // Update the block's metadata so the free function can find its order
        blocks[tile].prev = InUse | order;

        return tile;
    }

    // No tile :/
    return graphics::NoTile;
}

// Free a block, possibly getting its order
void BuddyObjectAllocator::freeBlock(u16 tile)
{
    // Bail out if no tile
    if (tile == graphics::NoTile) return;

    // Bail out if somehow we have been fed an already free block
    if (blocks[tile].prev == graphics::NoTile || !(blocks[tile].prev & InUse)) return;

    // Get the order and assure it is valid
    uint order = blocks[tile].prev &~ InUse;
    ASSERT(order <= LogNumTiles);

#ifdef CLEAR_TILE_AFTER
    // Just clear the tile, to see clearly what's going on
    memset32(&tile_mem_obj[0][tile], 0, (sizeof(TILE)/sizeof(u32)) << order);
#endif

    // Try to join the block with its buddy, to insert a superior block
    while (order < LogNumTiles)
    {
        u16 buddy = tile ^ (1 << order);

        // If its buddy is still active, stop joining
        if (blocks[buddy].prev & InUse) break;

        // Take the buddy from the free list, updating the links
        if (blocks[buddy].next != graphics::NoTile)
            blocks[blocks[buddy].next].prev = blocks[buddy].prev;
        if (blocks[buddy].prev != graphics::NoTile)
            blocks[blocks[buddy].prev].next = blocks[buddy].next;
        else freeList[order] = blocks[buddy].next;

        // Update the lists
        blocks[tile].next = graphics::NoTile;
        blocks[tile].prev = graphics::NoTile;
        blocks[buddy].next = graphics::NoTile;
        blocks[buddy].prev = graphics::NoTile;

        // Join the two blocks and increase the order
        tile &= ~(1 << order);
        order++;
    }

    // Inserts the block (possibly joined with its buddies) in the free list
    u16 first = freeList[order];
    if (first != graphics::NoTile)
        blocks[first].prev = tile;
    blocks[tile].next = first;
    blocks[tile].prev = graphics::NoTile;
    freeList[order] = tile;
}
