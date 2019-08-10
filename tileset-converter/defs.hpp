#pragma once

#include <cstdint>
#include <string>
#include <algorithm>

constexpr std::size_t TileSize = 16;
constexpr std::size_t HalfTileSize = TileSize / 2;

using Color = std::uint32_t;

template <typename T>
inline static auto toHex(T v)
{
    static const char* digits = "0123456789abcdef";

    std::string str;

    while (v)
    {
        auto digit = v & 0xF;
        str.push_back(digits[digit]);
        v >>= 4;
    }

    std::reverse(str.begin(), str.end());
    return str;
}
