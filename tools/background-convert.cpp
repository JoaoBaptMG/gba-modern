#include "pch.hpp"
#include "graphics.hpp"
#include "background-export-state.hpp"
#include "UnionFind.hpp"

// Calculates the union between two sets
auto setUnion(const std::vector<Color>& s1, const std::vector<Color>& s2)
{
    std::vector<Color> sresult(s1.size() + s2.size());
    sresult.erase(std::set_union(s1.begin(), s1.end(), 
        s2.begin(),s2.end(), sresult.begin()), sresult.end());
    return sresult;
}

ConversionResult convertBackgroundTo4bpp(const State<Character8bpp>& state8bpp,
    const std::array<Color, 256>& originalPalette, bool preserveOrder,
    const std::vector<std::size_t>& remapPalettes)
{
    // If preserveOrder is set, it will just check whether the tile has uniform tiles
    if (preserveOrder)
    {
        auto numChars = state8bpp.chars.size();
        State<Character4bpp> state;
        state.chars.resize(numChars);
        state.tiles.resize(state8bpp.tiles.width(), state8bpp.tiles.height());

        std::vector<std::size_t> paletteMaps(numChars);
        // Check for each character whether we have a common palette
        for (std::size_t i = 0; i < numChars; i++)
        {
            std::size_t palette = -1;
            for (auto b : state8bpp.chars[i])
            {
                // Ignore the zero color
                if (b == 0) continue;
                
                // Check if palette is the same
                std::size_t curPal = (std::size_t)b >> 4;
                if (palette == (std::size_t)-1) palette = curPal;
                else if (palette != curPal)
                    throw std::domain_error("Tile must have all colors of the same 16-color palette cluster!");
            }

            // Store the palette id
            if (palette == (std::size_t)-1) palette = 0;
            paletteMaps[i] = palette;

            // Write the new character
            for (std::size_t j = 0; j < CharSize4bpp; j++)
            {
                auto firstCode = state8bpp.chars[i][2*j] & 0xF;
                auto secondCode = state8bpp.chars[i][2*j+1] & 0xF;
                state.chars[i][j] = (firstCode) | (secondCode << 4);
            }
        }

        // Check the tiles so they can be corrected
        for (std::size_t j = 0; j < state8bpp.tiles.height(); j++)
            for (std::size_t i = 0; i < state8bpp.tiles.width(); i++)
            {
                // Get the actual tile id
                auto data = state8bpp.tiles(i, j);
                auto tile = data & 0x3FF;

                auto color = paletteMaps[tile];
                if (color >= remapPalettes.size())
                    throw std::domain_error("remap-palettes array provided is too small!");

                // And compose the palette map on it
                state.tiles(i, j) = data | (remapPalettes[color] << 12);
            }

        // Generate the palette file
        std::vector<Palette> palettes(remapPalettes.size(), Palette());
        for (std::size_t i = 0; i < remapPalettes.size(); i++)
            std::copy_n(originalPalette.begin() + 16*i, 16, palettes[i].begin());

        return { state, palettes };
    }
    else
    {
        // To begin with, we are going to build a set of all colors a tile uses
        auto numChars = state8bpp.chars.size();
        std::vector<std::vector<Color>> charColors(numChars);
        for (std::size_t i = 0; i < numChars; i++)
        {
            for (auto b : state8bpp.chars[i])
            {
                // Ignore the zero color
                if (b == 0) continue;
                charColors[i].push_back(originalPalette[b]);
            }

            // Sort and remove duplicates
            std::sort(charColors[i].begin(), charColors[i].end());
            charColors[i].erase(std::unique(charColors[i].begin(), charColors[i].end()), charColors[i].end());

            // Bail out if any of the characters has more colors than 4bpp can handle
            if (charColors[i].size() > 15)
                throw std::domain_error("There are characters which do not fit a 4bpp tileset!");
        }

        // Now, apply union-find to the colors while uniting them
        UnionFind sets(numChars);
        while (true)
        {
            constexpr std::size_t NoIndex = -1;
            std::size_t ci = NoIndex, cj = NoIndex;
            std::size_t minLength = 44;
            std::vector<Color> chUnion;

            // Check for pairs of chars to unite colors
            for (std::size_t j = 0; j < numChars-1; j++)
                for (std::size_t i = j+1; i < numChars; i++)
                {
                    // Don't unite already united colors
                    auto ki = sets.find(i);
                    auto kj = sets.find(j);
                    if (ki == kj) continue;

                    // Calculate their union
                    auto uni = setUnion(charColors[ki], charColors[kj]);
                    if (uni.size() > 15) continue;
                    if (minLength > uni.size())
                    {
                        ci = ki, cj = kj;
                        minLength = uni.size();
                        chUnion = std::move(uni);
                    }
                }
                
            // If no sets had been chosen, we can finish
            if (ci == NoIndex || cj == NoIndex) break;
            // Else, unite the sets
            else
            {
                sets.doUnion(ci, cj);
                charColors[sets.find(ci)] = std::move(chUnion);
            }
        }

        // If the number of unique palette sets is still too large, bail out
        if (sets.uniqueSets() > 16)
            throw std::domain_error("Could not find a suitable palette reduction for this tileset!");

        // Collect the unique set ids
        std::map<std::size_t, std::size_t> setIds;
        for (std::size_t i = 0; i < numChars; i++) 
            setIds[sets.find(i)] = 0;

        // Collect all the unique palettes
        std::vector<Palette> palettes(setIds.size(), Palette());
        std::size_t index = 0;
        for (auto& p : setIds)
        {
            const auto& palette = charColors[p.first];
            auto it = std::copy(palette.begin(), palette.end(), palettes[index].begin()+1);
            std::fill(it, palettes[index].end(), 0x7FFF);
            p.second = index++;
        }

        // Finally, we begin to construct the new state
        State<Character4bpp> state;
        std::vector<std::size_t> charCorrespondences(numChars);

        // Put the new characters first
        for (std::size_t i = 0; i < numChars; i++)
        {
            // Build the 4bpp version
            Character4bpp char4bpp;
            for (std::size_t j = 0; j < 32; j++)
            {
                const auto& pal = charColors[sets.find(i)];

                auto firstCode = state8bpp.chars[i][2*j];
                auto firstId = firstCode == 0 ? 0 :
                    std::lower_bound(pal.begin(), pal.end(), originalPalette[firstCode]) - pal.begin();
                
                auto secondCode = state8bpp.chars[i][2*j+1];
                auto secondId = secondCode == 0 ? 0 :
                    std::lower_bound(pal.begin(), pal.end(), originalPalette[secondCode]) - pal.begin();

                // Palettes are 1-based
                char4bpp[j] = (firstId+1) | ((secondId+1) << 4);
            }

            // Add it to the state and store its id
            charCorrespondences[i] = state.addCharacter(char4bpp);
        }

        // Assign the new tile indices
        state.tiles.resize(state8bpp.tiles.width(), state8bpp.tiles.height());
        for (std::size_t j = 0; j < state8bpp.tiles.height(); j++)
        {
            for (std::size_t i = 0; i < state8bpp.tiles.width(); i++)
            {
                // Decompose the tile index into its id and flags
                auto charId = state8bpp.tiles(i, j) & 0x3ff;

                // Put only the old flags in the state
                state.tiles(i, j) = state8bpp.tiles(i, j) ^ charId;

                // Compose the old flags with the new correspondent tile index
                // This is done as a bitwise-XOR operation because the
                // reversal flags behave as a XOR (reversed with reversed = normal)
                auto newId = charCorrespondences[charId];
                state.tiles(i, j) ^= newId;

                // Compose the palette in the last 4 bits of the index
                state.tiles(i, j) |= setIds[sets.find(charId)] << 12;

                // Since the first char is always empty, setting it to anything else
                // doesn't make sense
                if (newId == 0) state.tiles(i, j) = 0;
            }
        }

        return { state, palettes };
    }
}
