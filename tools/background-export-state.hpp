#pragma once

#include "pch.hpp"
#include "graphics.hpp"

template <typename Character>
struct State
{
    std::vector<Character> chars;
    std::map<Character, std::uint16_t> charIndices;
    util::grid<std::uint16_t> screenEntries;

    auto addCharacter(const Character& character, bool affine = false)
    {
        auto it = charIndices.find(character);
        if (it != charIndices.end()) return it->second;

        std::uint16_t sz = chars.size();
        chars.push_back(character);

        // Add the character
        charIndices.emplace(character, sz);
        if (!affine)
        {
            // Add all 3 flips of the character if it's not affine
            // If the character is symmetric, the respective flip will not be added
            charIndices.emplace(flipCharacter(character, Flip::Horizontal), sz | 0x400);
            charIndices.emplace(flipCharacter(character, Flip::Vertical), sz | 0x800);
            charIndices.emplace(flipCharacter(character, Flip::HorizontalVertical), sz | 0xC00);
        }

        return sz;
    }
};

using Palette = std::array<Color, 16>;

struct ConversionResult
{ 
    State<Character4bpp> state;
    std::vector<Palette> palettes;
};

ConversionResult convertBackgroundTo4bpp(const State<Character8bpp>& state8bpp,
    const std::array<Color, 256>& originalPalette, bool preserveOrder,
    const std::vector<std::size_t>& remapPalettes);

