//--------------------------------------------------------------------------------
// PaletteAllocator.hpp
//--------------------------------------------------------------------------------
// RAII wrapper around the allocation for palettes
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#include <cstdint>
#include "util/gba-assert.hpp"
#include "util/AllocatorPointer.hpp"
#include "graphics.hpp"

template <std::size_t N>
class PaletteAllocator final : public Allocator<PaletteAllocator<N>>
{
    const PALBANK* paletteData;
    u16 palettes[N];

    // Allocate the palette data and move it in place
    void alloc()
    { 
        graphics::allocateObjPalettes(N, palettes);
        for (std::size_t i = 0; i < N; i++)
            memcpy32(pal_obj_bank + palettes[i], paletteData+i, sizeof(PALBANK)/sizeof(u32));
    }

    // Free the data, optionally (for debug) clearing it
    void clear()
    {
        graphics::freeObjPalettes(N, palettes);
    }

public:
    PaletteAllocator(const void* paletteData) :
        paletteData((const PALBANK*)paletteData) {}

    auto getPalette(std::size_t i) const
    {
        ASSERT(i < N);
        return palettes[i];
    }

    template <typename Y>
    friend class Allocator;
};

template <std::size_t N>
class PalettePointer final : public AllocatorPointer<PaletteAllocator<N>>
{
public:
    INHERIT_ALLOCATOR_CTORS(PalettePointer, PaletteAllocator<N>)

    auto getPalette(std::size_t i = 0) const
    {
        ASSERT(this->allocator);
        return this->allocator->getPalette(i);
    }
};

using SinglePaletteAllocator = PaletteAllocator<1>;
using SinglePalettePointer = PalettePointer<1>;
