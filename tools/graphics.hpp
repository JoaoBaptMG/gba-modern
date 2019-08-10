#pragma once

#include "pch.hpp"

using Color = std::uint16_t;
inline static constexpr Color makeColor(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
    return (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10);
}

constexpr std::size_t CharSize4bpp = 32;
using Character4bpp = std::array<std::uint8_t, CharSize4bpp>;

constexpr std::size_t CharSize8bpp = 64;
using Character8bpp = std::array<std::uint8_t, CharSize8bpp>;

template <std::size_t CharSize, std::size_t PaletteSize>
struct CharacterData
{
    util::grid<std::array<std::uint8_t, CharSize>> chars;
    std::array<Color, PaletteSize> palette;
    std::size_t actualColors;
};

enum class Reverse { None, Horizontal, Vertical, HorizontalVertical };

using CharacterData4bpp = CharacterData<CharSize4bpp, 16>;
using CharacterData8bpp = CharacterData<CharSize8bpp, 256>;

CharacterData4bpp convertPngToCharacters4bpp(std::string filename, std::size_t maxColors = 16, bool preserveOrder = false);
CharacterData8bpp convertPngToCharacters8bpp(std::string filename, std::size_t maxColors = 256, bool preserveOrder = false);

Character4bpp reverseCharacter(Character4bpp ch, Reverse reverse);
Character8bpp reverseCharacter(Character8bpp ch, Reverse reverse);
