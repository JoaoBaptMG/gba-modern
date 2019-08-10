#include "pch.hpp"
#include "graphics.hpp"

struct State
{
    std::string inlabel;
    std::size_t groupWidth, groupHeight;
};

template <typename T>
void writeCharData(std::ostream& of, const State& state, const T& charData);
template <typename T>
void writeHeaderData(std::ostream& hof, const State& state, const T& charData);

int spriteExport(int argc, char **argv)
{
    if (argc < 5) throw std::out_of_range("sprite-export expects 3 arguments!");

    std::string in = argv[2];
    std::string out = argv[3];
    std::string outh = argv[4];

    // Parameters
    bool is8bpp = false;
    State state = { labelizeName(in), 1, 1 };
    std::ifstream mdin(in + ".json");
    std::size_t maxColors = 16;
    bool preserveOrder = false;
    if (mdin.good())
    {
        nlohmann::json j;
        mdin >> j;
        is8bpp = j.contains("is8bpp") && j.at("is8bpp").get<bool>();
        if (is8bpp) maxColors = 256;
        if (j.contains("group-width")) state.groupWidth = j.at("group-width").get<std::size_t>();
        if (j.contains("group-height")) state.groupHeight = j.at("group-height").get<std::size_t>();
        if (j.contains("max-colors")) maxColors = j.at("max-colors").get<std::size_t>();
        preserveOrder = j.contains("preserve-order") && j.at("preserve-order").get<bool>();
    }

    std::ofstream of;
    of.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    of.open(out);
    of << "@ " << std::endl;
    of << "@ " << out << std::endl;
    of << "@ " << std::endl << std::endl;

    std::ofstream hof;
    hof.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    hof.open(outh);
    hof << "// " << std::endl;
    hof << "// " << outh << std::endl;
    hof << "// " << std::endl;
    hof << "#pragma once" << std::endl << std::endl;
    hof << "#include <cstdint>" << std::endl << std::endl;

    if (!is8bpp)
    {
        auto charData = convertPngToCharacters4bpp(in, maxColors, preserveOrder);
        charData.palette[0] = 0;
        writeCharData(of, state, charData);
        writeHeaderData(hof, state, charData);
    }
    else
    {
        auto charData = convertPngToCharacters4bpp(in, maxColors, preserveOrder);
        charData.palette[0] = 0;
        writeCharData(of, state, charData);
        writeHeaderData(hof, state, charData);
    }

    of.close();
    hof.close();

    return 0;
}

template <typename T>
void writeCharData(std::ostream& of, const State& state, const T& charData)
{
    of << "    .section .rodata" << std::endl;
    of << "    .align 2" << std::endl;
    of << "    .global " << state.inlabel << "_tiles" << std::endl;
    of << "    .hidden " << state.inlabel << "_tiles" << std::endl;
    of << state.inlabel << "_tiles:";
    for (std::size_t j = 0; j < charData.chars.height(); j += state.groupHeight)
        for (std::size_t i = 0; i < charData.chars.width(); i += state.groupWidth)
        {
            std::size_t cellWidth = std::min(charData.chars.width() - i, state.groupWidth);
            std::size_t cellHeight = std::min(charData.chars.height() - j, state.groupHeight);
            for (const auto& c : charData.chars.make_view(i, j, cellWidth, cellHeight))
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
        }

    of << std::endl << std::endl;
    of << "    .section .rodata" << std::endl;
    of << "    .align 2" << std::endl;
    of << "    .global " << state.inlabel << "_palette" << std::endl;
    of << "    .hidden " << state.inlabel << "_palette" << std::endl;
    of << state.inlabel << "_palette:";

    std::size_t index = 0;
    for (Color c : charData.palette)
    {
        if (index % 16 == 0) of << std::endl << "    .hword ";
        else of << ", ";
        of << toHex(c, 4);
        index++;
    }
    of << std::endl << std::endl;
}

template <typename T>
void writeHeaderData(std::ostream& hof, const State& state, const T& charData)
{
    hof << "extern const std::uint8_t " << state.inlabel << "_tiles[";
    std::size_t size = charData.chars.width() * charData.chars.height() * charData.chars.at(0, 0).size();
    hof << size << "];" << std::endl;

    hof << "extern const std::uint16_t " << state.inlabel << "_palette[";
    hof << charData.palette.size() << "];" << std::endl;
}
