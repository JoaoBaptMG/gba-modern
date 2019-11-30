//--------------------------------------------------------------------------------
// LightBitset.hpp
//--------------------------------------------------------------------------------
// Provides a lightweight bitset that does not do memory allocation
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>

template <std::size_t N>
class LightBitset final
{
    static constexpr std::size_t NumCells = (N + 31) >> 5;
    std::uint32_t cells[NumCells];

public:
    // Default constructors are okay
    bool test(std::size_t i) const { return cells[i>>5] & (1 << (i&31)); }
    void set(std::size_t i) { cells[i>>5] |= 1 << (i&31); }
    void reset(std::size_t i) { cells[i>>5] &= ~(1 << (i&31)); }

    void setAll() { for (auto& cell : cells) cell = -1; }
    void resetAll() { for (auto& cell : cells) cell = 0; }
};
