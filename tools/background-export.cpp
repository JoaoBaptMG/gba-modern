#include "pch.hpp"
#include "background-export-state.hpp"

struct Offset { int x, y; };

struct GlobalState
{
    bool exportPalettes = true;
    bool exportSizes = true;
    bool staticCharSize = false;
    std::size_t groupWidth = 1;
    std::size_t groupHeight = 1;
};

template <typename Character>
void writeBackground(std::ostream& of, const SpecialName& name, const State<Character>& state,
    const std::vector<Palette>& palettes, const GlobalState& gstate);

int backgroundExport(int argc, char **argv)
{
    if (argc < 5)
    {
        throw std::invalid_argument("background-export requires 3 arguments!");
    }

    std::string in = argv[2];
    std::string out = argv[3];
    std::string outh = argv[4];

    // Load the companion .json file
    std::ifstream inf;
    inf.open(in + ".json");

    // Get the main attributes (if the file is loaded)
    bool is8bpp = false;
    bool preserveOrder = false;
    std::vector<std::size_t> remapPalettes{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    GlobalState gstate;
    if (inf.good())
    {
        nlohmann::json j;
        inf >> j;
        if (j.contains("is8bpp")) j.at("is8bpp").get_to(is8bpp);
        if (j.contains("export-palettes")) j.at("export-palettes").get_to(gstate.exportPalettes);
        if (j.contains("export-sizes")) j.at("export-sizes").get_to(gstate.exportSizes);
        if (j.contains("group-width")) j.at("export-sizes").get_to(gstate.groupWidth);
        if (j.contains("group-height")) j.at("export-sizes").get_to(gstate.groupHeight);
        if (j.contains("static-char-size")) j.at("static-char-size").get_to(gstate.staticCharSize);
        if (j.contains("preserve-order")) j.at("preserve-order").get_to(preserveOrder);
        if (preserveOrder && j.contains("remap-palettes")) j.at("remap-palettes").get_to(remapPalettes);
    }

    // Load the image
    auto image = convertImageToCharacters8bpp(loadPngToImage(in), 256, preserveOrder);

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
        writeBackground(of, name, state, palettes, gstate);
    }
    else
    {
        // Do the conversion and write
        if (preserveOrder)
        {
            auto numPalettes = (image.actualColors + 15) / 16;
            if (remapPalettes.size() < numPalettes)
                throw std::domain_error("remap-palettes array provided is too small!");
            remapPalettes.resize(numPalettes);
        }

        auto [state4bpp, palettes] = convertBackgroundTo4bpp(state, image.palette, preserveOrder, remapPalettes);
        writeBackground(of, name, state4bpp, palettes, gstate);
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
        hof << "    extern const BackgroundHandle<";
        if (gstate.staticCharSize)
            hof << (state.chars.size() * (is8bpp ? sizeof(Character8bpp) : sizeof(Character4bpp))) << ", ";
        else hof << "(std::size_t)-1, ";
        hof << (gstate.exportSizes ? "true" : "false") << ", ";
        hof << (gstate.exportPalettes ? "true" : "false") << "> ";
        hof << name.fileName << ';' << std::endl;
        hof << "}" << std::endl << std::endl;
    }

    return 0;
}

template <typename Character>
void writeBackground(std::ostream& of, const SpecialName& name, const State<Character>& state,
    const std::vector<Palette>& palettes, const GlobalState& gstate)
{
    // The main structure
    of << "    .section .rodata" << std::endl;
    of << "    .align 2" << std::endl;
    of << "    .global " << name.mangledName << std::endl;
    of << "    .hidden " << name.mangledName << std::endl;
    of << name.mangledName << ":" << std::endl;

    // Export the metadata
    constexpr bool is8bpp = std::is_same<Character, Character8bpp>::value;

    of << "    .word ts_" << name.fileName << "_chars, ts_" << name.fileName << "_tiles" << std::endl;
    if (!gstate.staticCharSize)
        of << "    .word " << (state.chars.size() * sizeof(Character)) << std::endl;

    if (gstate.exportSizes) of << "    .hword " << state.tiles.width() << ", " << state.tiles.height() << std::endl;
    if (gstate.exportPalettes)
    {
        of << "    .hword " << palettes.size() << ", " << (int)is8bpp << std::endl;
        of << "    .word ts_" << name.fileName << "_palette" << std::endl;
    }

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
    of << "ts_" << name.fileName << "_tiles:";
    std::size_t id = 0;
    for (std::size_t j = 0; j < state.tiles.height(); j += gstate.groupHeight)
        for (std::size_t i = 0; i < state.tiles.width(); i += gstate.groupWidth)
        {
            std::size_t cellWidth = std::min(state.tiles.width() - i, gstate.groupWidth);
            std::size_t cellHeight = std::min(state.tiles.height() - j, gstate.groupHeight);
            for (const auto& tile : state.tiles.make_view(i, j, cellWidth, cellHeight))
            {
                if (id % 8 == 0) of << std::endl << "    .hword ";
                else of << ", ";
                of << toHex(tile, 4);
                id++;
            }
        }

    if (gstate.exportPalettes)
    {
        of << std::endl << std::endl;
        of << "    .section .rodata" << std::endl;
        of << "    .align 2" << std::endl;
        of << "    .hidden ts_" << name.fileName << "_palette" << std::endl;
        of << "ts_" << name.fileName << "_palette:";

        for (const auto& palette : palettes)
        {
            std::size_t id = 0;
            for (Color c : palette)
            {
                if (id % 8 == 0) of << std::endl << "    .hword ";
                else of << ", ";
                of << toHex(c, 4);
                id++;
            }
            of << std::endl;
        }
    }

    of << std::endl;
}