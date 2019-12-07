#include "pch.hpp"
#include "tileset-export-state.hpp"

struct Offset { int x, y; };

void processAutotile(const CharacterData8bpp& image, State<Character8bpp>& state, std::size_t id, bool solid);
void processTileset(const CharacterData8bpp& image, State<Character8bpp>& state, const std::vector<bool>& solid);

template <typename Character>
void writeTileset(std::ostream& of, const SpecialName& name, const State<Character>& state, const std::vector<Palette>& palettes);

int tilesetExport(int argc, char **argv)
{
    if (argc < 5)
    {
        throw std::invalid_argument("tileset-export requires 3 arguments!");
    }

    std::string in = argv[2];
    std::string out = argv[3];
    std::string outh = argv[4];

    // Load the image
    auto image = convertPngToCharacters8bpp(in);

    // Guarantee the right size
    if (image.chars.width() != 16)
        throw std::domain_error("Tileset image must be 256 pixels wide!");
    if (image.chars.height() % 2 != 0)
        throw std::domain_error("Tileset image must have height multiple of 16 pixels!");
    if (image.chars.height() < 6)
        throw std::domain_error("Tileset image must be at least 48 pixels high!");

    // Load the companion .json file
    std::ifstream inf;
    inf.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    inf.open(in + ".json");
    nlohmann::json j;
    inf >> j;

    // Get the main attributes
    bool is8bpp = j.contains("is8bpp") && j.at("is8bpp").get<bool>();

    std::size_t numAutotiles = 0;
    std::vector<bool> autotilesSolid;
    if (j.contains("num-autotiles")) j.at("num-autotiles").get_to(numAutotiles);
    if (j.contains("autotiles-solid")) j.at("autotiles-solid").get_to(autotilesSolid);

    if (numAutotiles != autotilesSolid.size())
        throw std::domain_error("The size the 'autotiles-solid' array must be equal to 'num-autotiles'!");

    auto tilesetSolid = j.at("tileset-solid").get<std::vector<bool>>();

    State<Character8bpp> state;
    auto emptyid = state.addCharacter(Character8bpp());
    state.tiles.push_back({ emptyid, emptyid, emptyid, emptyid });
    state.flags.push_back(0);

    for (std::size_t i = 0; i < numAutotiles; i++)
        processAutotile(image, state, i, autotilesSolid.at(i));
    processTileset(image, state, tilesetSolid);

    // Check validity
    if (state.chars.size() > 1024)
        throw std::out_of_range("Maximum number of generated characters exceeded (1024).");

    if (state.tiles.size() > 256)
        throw std::out_of_range("Maximum number of generated tiles exceeded (255).");

    state.tiles.resize(256, { emptyid, emptyid, emptyid, emptyid });
    state.flags.resize(256, 0);

    std::ofstream of;
    of.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    of.open(out);

    // Write the main file
    of << "@ " << std::endl;
    of << "@ " << out << std::endl;
    of << "@ " << std::endl << std::endl;

    auto name = deriveSpecialName(out);

    if (is8bpp)
    {
        // Generate the palette file
        auto numPalettes = (image.actualColors + 15) / 16;
        std::vector<Palette> palettes(numPalettes, Palette());
        for (std::size_t i = 0; i < numPalettes; i++)
        {
            auto numColors = std::min(image.actualColors - 16*i, std::size_t(16));
            std::copy_n(image.palette.begin() + 16*i, numColors, palettes[i].begin());
        }

        // Set the first color to zero
        palettes[0][0] = 0;

        // And write the file
        writeTileset(of, name, state, palettes);
    }
    else
    {
        // Do the conversion and write
        auto [state4bpp, palettes] = convertTilesetTo4bpp(state, image.palette);
        writeTileset(of, name, state4bpp, palettes);
    }

    // Write the header
    {
        std::ofstream hof;
        hof.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        hof.open(outh);
        hof << "// " << std::endl;
        hof << "// " << outh << std::endl;
        hof << "// " << std::endl;
        hof << "#pragma once" << std::endl << std::endl;
        hof << "namespace " << name.nmspace << std::endl;
        hof << "{" << std::endl;
        hof << "    extern const TilesetData " << name.fileName << ';' << std::endl;
        hof << "}" << std::endl << std::endl;
    }

    return 0;
}

extern Offset autotileOffsets[47][4];

void processAutotile(const CharacterData8bpp& image, State<Character8bpp>& state, std::size_t id, bool solid)
{
    for (const auto& at : autotileOffsets)
    {
        std::array<std::uint16_t, 4> charIndices;

        for (int i = 0; i < 4; i++)
            charIndices[i] = state.addCharacter(image.chars.at(4*id + at[i].x, at[i].y));
        
        state.tiles.push_back(std::move(charIndices));
        state.flags.push_back(solid);
    }
}

void processTileset(const CharacterData8bpp& image, State<Character8bpp>& state, const std::vector<bool>& solid)
{
    std::size_t w = 8;
    std::size_t h = image.chars.height()/2;

    for (std::size_t j = 3; j < h; j++)
        for (std::size_t i = 0; i < w; i++)
        {
            std::array<std::uint16_t, 4> charIndices;
            charIndices[0] = state.addCharacter(image.chars.at(2*i+0, 2*j+0));
            charIndices[1] = state.addCharacter(image.chars.at(2*i+1, 2*j+0));
            charIndices[2] = state.addCharacter(image.chars.at(2*i+0, 2*j+1));
            charIndices[3] = state.addCharacter(image.chars.at(2*i+1, 2*j+1));
            state.tiles.push_back(std::move(charIndices));

            auto kid = (j - 3) * w + i;
            state.flags.push_back(kid < solid.size() && solid[kid] ? 0x1 : 0x0);
        }
}

template <typename Character>
void writeTileset(std::ostream& of, const SpecialName& name, const State<Character>& state, const std::vector<Palette>& palettes)
{
    // The main structure
    of << "    .section .rodata" << std::endl;
    of << "    .align 2" << std::endl;
    of << "    .global " << name.mangledName << std::endl;
    of << "    .hidden " << name.mangledName << std::endl;
    of << name.mangledName << ":" << std::endl;
    of << std::endl << "    @ Tiles";

    // Export the tiles
    std::size_t id = 0;
    for (const auto& tile : state.tiles)
    {
        if (id % 4 == 0) of << std::endl << "    .hword ";
        else of << ", ";
        of << toHex(tile[0], 4) << ", " << toHex(tile[1], 4) << ", ";
        of << toHex(tile[2], 4) << ", " << toHex(tile[3], 4);
        id++;
    }

    // Export the flags
    of << std::endl << std::endl << "    @ Flags";
    id = 0;
    for (const auto& flag : state.flags)
    {
        if (id % 16 == 0) of << std::endl << "    .byte ";
        else of << ", ";
        of << toHex(flag, 2);
        id++;
    }

    constexpr bool is8bpp = std::is_same<Character, Character8bpp>::value;
    // Export the rest of the data
    of << std::endl << std::endl;
    of << "    .hword " << (state.chars.size() * sizeof(Character)) << std::endl;
    of << "    .byte " << palettes.size();
    of << ", " << toHex(is8bpp ? 1 : 0, 2) << std::endl;
    of << "    .word ts_" << name.fileName << "_chars, ts_" << name.fileName << "_palette" << std::endl;

    // Now, the chars and the palettes
    of << std::endl;
    of << "    .section .rodata" << std::endl;
    of << "    .align 2" << std::endl;
    of << "    .hidden ts_" << name.fileName << "_chars" << std::endl;
    of << "ts_" << name.fileName << "_chars:";
    for (const auto& c : state.chars)
    {
        std::size_t index = 0;
        for (std::uint8_t b : c)
        {
            if (index % 16 == 0) of << std::endl << "    .byte ";
            else of << ", ";
            of << toHex(b, 2);
            index++;
        }
    }

    of << std::endl << std::endl;
    of << "    .section .rodata" << std::endl;
    of << "    .align 2" << std::endl;
    of << "    .hidden ts_" << name.fileName << "_palette" << std::endl;
    of << "ts_" << name.fileName << "_palette:" << std::endl;

    for (const auto& palette : palettes)
    {
        of << "    .hword ";
        bool nf = false;
        for (Color c : palette)
        {
            if (nf) of << ", ";
            of << toHex(c, 4);
            nf = true;
        }
        of << std::endl;
    }
    of << std::endl;
}

Offset autotileOffsets[47][4] =
{
    // Corners
    { {2,4},{1,4},{2,3},{1,3} },
    { {2,0},{1,4},{2,3},{1,3} },
    { {2,4},{3,0},{2,3},{1,3} },
    { {2,0},{3,0},{2,3},{1,3} },
    { {2,4},{1,4},{2,3},{3,1} },
    { {2,0},{1,4},{2,3},{3,1} },
    { {2,4},{3,0},{2,3},{3,1} },
    { {2,0},{3,0},{2,3},{3,1} },
    { {2,4},{1,4},{2,1},{1,3} },
    { {2,0},{1,4},{2,1},{1,3} },
    { {2,4},{3,0},{2,1},{1,3} },
    { {2,0},{3,0},{2,1},{1,3} },
    { {2,4},{1,4},{2,1},{3,1} },
    { {2,0},{1,4},{2,1},{3,1} },
    { {2,4},{3,0},{2,1},{3,1} },
    { {2,0},{3,0},{2,1},{3,1} },

    // Sides
    { {0,4},{1,4},{0,3},{1,3} },
    { {0,4},{3,0},{0,3},{1,3} },
    { {0,4},{1,4},{0,3},{3,1} },
    { {0,4},{3,0},{0,3},{3,1} },
    { {2,2},{1,2},{2,3},{1,3} },
    { {2,2},{1,2},{2,3},{3,1} },
    { {2,2},{1,2},{2,1},{1,3} },
    { {2,2},{1,2},{2,1},{3,1} },
    { {2,4},{3,4},{2,3},{3,3} },
    { {2,4},{3,4},{2,1},{3,3} },
    { {2,0},{3,4},{2,3},{3,3} },
    { {2,0},{3,4},{2,1},{3,3} },
    { {2,4},{1,4},{2,5},{1,5} },
    { {2,0},{1,4},{2,5},{1,5} },
    { {2,4},{3,0},{2,5},{1,5} },
    { {2,0},{3,0},{2,5},{1,5} },

    // Pipes
    { {0,4},{3,4},{0,3},{3,3} },
    { {2,2},{1,2},{2,5},{1,5} },

    // Real corners
    { {0,2},{1,2},{0,3},{1,3} },
    { {0,2},{1,2},{0,3},{3,1} },
    { {2,2},{3,2},{2,3},{3,3} },
    { {2,2},{3,2},{2,1},{3,3} },
    { {2,4},{3,4},{2,5},{3,5} },
    { {2,0},{3,4},{2,5},{3,5} },
    { {0,4},{1,4},{0,5},{1,5} },
    { {0,4},{3,0},{0,5},{1,5} },

    // Dead-ends
    { {0,2},{3,2},{0,3},{3,3} },
    { {0,2},{1,2},{0,5},{1,5} },
    { {0,4},{3,4},{0,5},{3,5} },
    { {2,2},{3,2},{2,5},{3,5} },

    // Single block
    { {0,2},{3,2},{0,5},{3,5} }
};
