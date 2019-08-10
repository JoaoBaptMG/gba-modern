#pragma once

#include "pch.hpp"
#include "graphics.hpp"

template <typename Character>
struct State
{
    std::vector<Character> chars;
    std::map<Character, std::uint16_t> charIndices;

    std::vector<std::array<std::uint16_t, 4>> tiles;
    std::vector<std::uint8_t> flags;

    auto addCharacter(const Character& character)
    {
        auto it = charIndices.find(character);
        if (it != charIndices.end()) return it->second;

        std::uint16_t sz = chars.size();
        chars.push_back(character);

        // Add all 4 reversions of the character
        // If the character is symmetric, the respective reversion will not be added
        charIndices.emplace(character, sz);
        charIndices.emplace(reverseCharacter(character, Reverse::Horizontal), sz | 0x400);
        charIndices.emplace(reverseCharacter(character, Reverse::Vertical), sz | 0x800);
        charIndices.emplace(reverseCharacter(character, Reverse::HorizontalVertical), sz | 0xC00);

        return sz;
    }
};

using Palette = std::array<Color, 16>;

struct ConversionResult
{ 
    State<Character4bpp> state;
    std::vector<Palette> palettes;
};

ConversionResult convertTilesetTo4bpp(const State<Character8bpp>& state8bpp,
    const std::array<Color, 256>& originalPalette);

