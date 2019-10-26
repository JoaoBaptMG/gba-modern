//--------------------------------------------------------------------------------
// BuddyObjAlloc.cpp
//--------------------------------------------------------------------------------
// Provide a tile memory allocator for the VRAM, for the object tiles
//--------------------------------------------------------------------------------

#include "BuddyObjAlloc.hpp"
#include "util/gba-assert.hpp"

// In use flag
constexpr u16 InUse = 0x8000;

BuddyObjectAllocator::BuddyObjectAllocator()
{
    // Populate all the free list - UB, I know, but I don't care
    constexpr uint Size = sizeof(blocks) + sizeof(freeList);
    memset32(blocks, dup16(NoTile), Size / sizeof(u32));

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
        if (freeList[foundOrder] != NoTile) break;

    // Now, if we found a block, split if it required
    if (foundOrder <= LogNumTiles)
    {
        // Take the block away from the free list, and update the links
        u16 tile = freeList[foundOrder];
        freeList[foundOrder] = blocks[tile].next;
        if (blocks[tile].next != NoTile)
            blocks[blocks[tile].next].prev = NoTile;

        // If the order was a superior one, split the block
        while (foundOrder > order)
        {
            // Descend to the previous order
            foundOrder--;

            // Create the buddy block and puts it into the free list
            u16 buddy = tile + (1 << foundOrder);
            u16 first = freeList[foundOrder];
            if (first != NoTile)
                blocks[first].prev = buddy;
            blocks[buddy].next = first;
            blocks[buddy].prev = NoTile;
            freeList[foundOrder] = buddy;

            // Now we have a block at tile, but with a order lowererd by one
        }

        // Update the block's metadata so the free function can find its order
        blocks[tile].prev = InUse | order;

        return tile;
    }

    // No tile :/
    return NoTile;
}

void BuddyObjectAllocator::freeBlock(u16 tile)
{
    // Bail out if somehow we have been fed an already free block
    if (blocks[tile].prev == NoTile || !(blocks[tile].prev & InUse) ||
        blocks[tile].next == NoTile || !(blocks[tile].next & InUse)) return;

    // Get the order and assure it is valid
    uint order = blocks[tile].prev &~ InUse;
    ASSERT(order <= LogNumTiles);

    // Try to join the block with its buddy, to insert a superior block
    while (order < LogNumTiles)
    {
        u16 buddy = tile ^ (1 << order);

        // If its buddy is still active, stop joining
        if (blocks[buddy].prev & InUse) break;

        // Take the buddy from the free list, updating the links
        if (blocks[buddy].next != NoTile)
            blocks[blocks[buddy].next].prev = blocks[buddy].prev;
        if (blocks[buddy].prev != NoTile)
            blocks[blocks[buddy].prev].next = blocks[buddy].next;
        else freeList[order] = blocks[buddy].next;

        // Update the lists
        blocks[tile].next = NoTile;
        blocks[tile].prev = NoTile;
        blocks[buddy].next = NoTile;
        blocks[buddy].prev = NoTile;

        // Join the two blocks and increase the order
        tile &= ~(1 << order);
        order++;
    }

    // Inserts the block (possibly joined with its buddies) in the free list
    u16 first = freeList[order];
    if (first != NoTile)
        blocks[first].prev = tile;
    blocks[tile].next = first;
    blocks[tile].prev = NoTile;
    freeList[order] = tile;
}
