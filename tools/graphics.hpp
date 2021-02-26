#pragma once

#include "pch.hpp"

using Color = std::uint16_t;
inline static constexpr Color makeColor(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
    return (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10);
}

struct Image8bpp
{
    std::vector<std::uint8_t> image;
    unsigned int width, height;
    std::vector<Color> palette;

    std::uint8_t operator()(std::size_t x, std::size_t y) const
    {
        return image[width*y + x];
    }
};

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

enum class Flip { None, Horizontal, Vertical, HorizontalVertical };

using CharacterData4bpp = CharacterData<CharSize4bpp, 16>;
using CharacterData8bpp = CharacterData<CharSize8bpp, 256>;

struct Bitmask
{
    std::vector<std::uint16_t> data;
    std::size_t hwidth, hheight;
};

Image8bpp loadPngToImage(std::string filename);
CharacterData4bpp convertImageToCharacters4bpp(const Image8bpp& image, std::size_t maxColors = 16, bool preserveOrder = false);
CharacterData8bpp convertImageToCharacters8bpp(const Image8bpp& image, std::size_t maxColors = 256, bool preserveOrder = false);
Bitmask generateImageBitmask(const Image8bpp& image);

Character4bpp flipCharacter(Character4bpp ch, Flip flip);
Character8bpp flipCharacter(Character8bpp ch, Flip flip);
