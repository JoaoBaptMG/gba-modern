#include "pch.hpp"
#include "graphics.hpp"

struct State
{
    std::string inlabel;
    std::size_t groupWidth, groupHeight;
};

struct AnimationPose
{
    std::size_t init, repeat, end;
};

void from_json(const nlohmann::json& j, AnimationPose& af)
{
    j.at(0).get_to(af.init);
    j.at(1).get_to(af.repeat);
    j.at(2).get_to(af.end);
}

template <typename T>
void writeCharData(std::ostream& of, const State& state, const T& charData);
template <typename T>
void writeHeaderData(std::ostream& hof, const State& state, const T& charData);

using AnimationData = std::map<std::string, AnimationPose>;
void writeAnimationData(std::ostream &hof, const State &state, const AnimationData &animations, std::size_t frameStep,
    std::size_t totalNumFrames);

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
    bool exportAnimation = false;
    AnimationData animations;
    std::size_t frameStep;
    if (mdin.good())
    {
        nlohmann::json j;
        mdin >> j;
        is8bpp = j.contains("is8bpp") && j.at("is8bpp").get<bool>();
        if (is8bpp) maxColors = 256;
        if (j.contains("group-width")) j.at("group-width").get_to(state.groupWidth);
        if (j.contains("group-height")) j.at("group-height").get_to(state.groupHeight);
        if (j.contains("max-colors")) j.at("max-colors").get_to(maxColors);
        if (j.contains("preserve-order")) j.at("preserve-order").get_to(preserveOrder);
        if (j.contains("animation-poses") && j.contains("animation-frames"))
        {
            exportAnimation = true;
            j.at("animation-poses").get_to(animations);
            j.at("animation-frames").get_to(frameStep);
        }
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
    hof << "#include <cstdint>" << std::endl;
    if (exportAnimation)
    {
        hof << "#include <utility>" << std::endl;
        hof << "#include \"graphics/AnimationPose.hpp\"" << std::endl;
    }
    hof << std::endl;

    std::size_t numFrames;
    if (!is8bpp)
    {
        auto charData = convertPngToCharacters4bpp(in, maxColors, preserveOrder);
        charData.palette[0] = 0;
        writeCharData(of, state, charData);
        writeHeaderData(hof, state, charData);

        auto framesX = (charData.chars.width() + state.groupWidth - 1) / state.groupWidth;
        auto framesY = (charData.chars.height() + state.groupHeight - 1) / state.groupHeight;
        numFrames = framesX * framesY;
    }
    else
    {
        auto charData = convertPngToCharacters8bpp(in, maxColors, preserveOrder);
        charData.palette[0] = 0;
        writeCharData(of, state, charData);
        writeHeaderData(hof, state, charData);

        auto framesX = (charData.chars.width() + state.groupWidth - 1) / state.groupWidth;
        auto framesY = (charData.chars.height() + state.groupHeight - 1) / state.groupHeight;
        numFrames = framesX * framesY;
    }

    if (exportAnimation) writeAnimationData(hof, state, animations, frameStep, numFrames);

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

std::vector<std::size_t> powersOfTwo(std::size_t k)
{
    std::size_t pot = 8*sizeof(std::size_t) - 1;
    std::size_t bit = (std::size_t)1 << pot;

    std::vector<std::size_t> bits;
    while (bit)
    {
        if (k & bit) bits.push_back(pot);
        bit >>= 1;
        pot--;
    }

    return bits;
}

void writeAnimationData(std::ostream &hof, const State &state, const AnimationData &animations, std::size_t frameStep,
    std::size_t totalNumFrames)
{
    hof << std::endl;
    hof << "namespace " << state.inlabel << "_animation" << std::endl;
    hof << '{' << std::endl;
    hof << "    constexpr std::size_t FrameStep = " << frameStep << ';' << std::endl;
    hof << "    using AllocationBlocks = std::index_sequence<";

    bool nf = false;
    for (auto b : powersOfTwo(totalNumFrames))
    {
        if (nf) hof << ", ";
        nf = true;
        hof << b;
    }

    hof << ">;" << std::endl << std::endl;
    for (const auto& p : animations)
    {
        hof << "    constexpr AnimationPose Animation_" << p.first << " = { ";
        hof << p.second.init << ", " << p.second.repeat << ", " << p.second.end << " };" << std::endl;
    }
    hof << '}' << std::endl;
}