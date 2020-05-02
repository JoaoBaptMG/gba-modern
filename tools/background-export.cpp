#include "pch.hpp"
#include "background-export-state.hpp"

struct Offset { int x, y; };

template <typename Character>
void writeBackground(std::ostream& of, const SpecialName& name, const State<Character>& state, const std::vector<Palette>& palettes);

int backgroundExport(int argc, char **argv)
{
    if (argc < 5)
    {
        throw std::invalid_argument("background-export requires 3 arguments!");
    }

    std::string in = argv[2];
    std::string out = argv[3];
    std::string outh = argv[4];

    // Load the image
    auto image = convertImageToCharacters8bpp(loadPngToImage(in));

    // Load the companion .json file
    std::ifstream inf;
    inf.open(in + ".json");

    // Get the main attributes (if the file is loaded)
    bool is8bpp = false;
    if (inf.good())
    {
        nlohmann::json j;
        inf >> j;
        if (j.contains("is8bpp")) j.at("is8bpp").get_to(is8bpp);
    }

    State<Character8bpp> state;
    auto width = image.chars.width(), height = image.chars.height();
    state.tiles.resize(width, height);

    // Add all the tiles to the state
    for (std::size_t j = 0; j < height; j++)
        for (std::size_t i = 0; i < width; i++)
            state.tiles(i, j) = state.addCharacter(image.chars(i, j));

    // Check validity
    std::size_t maxSize = is8bpp ? 512 : 1024;
    if (state.chars.size() > maxSize)
        throw std::out_of_range("Maximum number of generated characters exceeded (" + std::to_string(maxSize) + ").");

    std::ofstream of;
    of.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    of.open(out);

    // Write the main file
    of << "@ " << std::endl;
    of << "@ " << out << std::endl;
    of << "@ " << std::endl << std::endl;

    auto name = deriveSpecialName(in);

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
        writeBackground(of, name, state, palettes);
    }
    else
    {
        // Do the conversion and write
        auto [state4bpp, palettes] = convertBackgroundTo4bpp(state, image.palette);
        writeBackground(of, name, state4bpp, palettes);
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
        hof << "#include \"data/BackgroundData.hpp\"" << std::endl << std::endl;
        hof << "namespace " << name.nmspace << std::endl;
        hof << "{" << std::endl;
        hof << "    extern const BackgroundHandle " << name.fileName << ';' << std::endl;
        hof << "}" << std::endl << std::endl;
    }

    return 0;
}

template <typename Character>
void writeBackground(std::ostream& of, const SpecialName& name, const State<Character>& state, const std::vector<Palette>& palettes)
{
    // The main structure
    of << "    .section .rodata" << std::endl;
    of << "    .align 2" << std::endl;
    of << "    .global " << name.mangledName << std::endl;
    of << "    .hidden " << name.mangledName << std::endl;
    of << name.mangledName << ":" << std::endl;

    // Export the metadata
    constexpr bool is8bpp = std::is_same<Character, Character8bpp>::value;
    of << "    .hword " << state.tiles.width() << ", " << state.tiles.height() << std::endl;
    of << "    .hword " << (state.chars.size() * sizeof(Character));
    of << ", " << toHex((is8bpp << 15) | palettes.size(), 4) << std::endl;
    of << "    .word ts_" << name.fileName << "_chars, ts_" << name.fileName;
    of << "_tiles, ts_" << name.fileName << "_palette" << std::endl;

    // Now, the chars, tiles and palettes
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
    of << "    .hidden ts_" << name.fileName << "_tiles" << std::endl;
    of << "ts_" << name.fileName << "_tiles:" << std::endl;
    std::size_t id = 0;
    for (const auto& tile : state.tiles)
    {
        if (id % 16 == 0) of << std::endl << "    .hword ";
        else of << ", ";
        of << toHex(tile, 4);
        id++;
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