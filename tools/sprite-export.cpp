#include "pch.hpp"
#include "graphics.hpp"

struct State
{
    SpecialName name;
    std::size_t groupWidth, groupHeight;
    bool exportPalette;
};

struct AnimationPose
{
    std::size_t init, repeat, end;
};

void from_json(const nlohmann::json& j, AnimationPose& af)
{
    // This accepts three forms
    // [x], a single frame that gets animated
    // [i, e], where the animation loops from i to e
    // [i, r, e], where the animation executes once from i to e, then loops back from r to e
    std::size_t iinit = 0;
    std::size_t irepeat = std::max((int)j.size()-2, 0);
    std::size_t iend = j.size()-1;

    j.at(iinit).get_to(af.init);
    j.at(irepeat).get_to(af.repeat);
    j.at(iend).get_to(af.end);
}

template <typename T>
void writeCharData(std::ostream& of, const State& state, const T& charData);
template <typename T>
void writeHeaderData(std::ostream& hof, const State& state, const T& charData, std::size_t bitmaskSize);

void writeCharAnimationData(std::ofstream& of, const State& state, std::size_t totalNumFrames);

using AnimationData = std::map<std::string, AnimationPose>;
template <typename T>
void writeAnimatedHeaderData(std::ostream &hof, const State &state, const T& charData,
    const AnimationData &animations, std::size_t frameStep, std::size_t totalNumFrames, std::size_t bitmaskSize);

void writeBitmaskData(std::ostream& of, const Bitmask& bitmask);

int spriteExport(int argc, char **argv)
{
    if (argc < 5) throw std::out_of_range("sprite-export expects 3 arguments!");

    std::string in = argv[2];
    std::string out = argv[3];
    std::string outh = argv[4];

    // Parameters
    bool is8bpp = false;
    State state = { deriveSpecialName(in), 1, 1, true };
    std::ifstream mdin(in + ".json");
    std::size_t maxColors = 16;
    bool preserveOrder = false;
    bool exportAnimation = false;
    bool generateBitmask = false;
    AnimationData animations;
    std::size_t frameStep;
    if (mdin.good())
    {
        nlohmann::json j;
        mdin >> j;
        if (j.contains("is8bpp")) j.at("is8bpp").get_to(is8bpp);
        if (is8bpp) maxColors = 256;
        if (j.contains("group-width")) j.at("group-width").get_to(state.groupWidth);
        if (j.contains("group-height")) j.at("group-height").get_to(state.groupHeight);
        if (j.contains("max-colors")) j.at("max-colors").get_to(maxColors);
        if (j.contains("preserve-order")) j.at("preserve-order").get_to(preserveOrder);
        if (j.contains("export-palette")) j.at("export-palette").get_to(state.exportPalette);
        if (j.contains("generate-bitmask")) j.at("generate-bitmask").get_to(generateBitmask);
        if (j.contains("animation-poses") && j.contains("animation-frames"))
        {
            exportAnimation = true;
            j.at("animation-poses").get_to(animations);
            j.at("animation-frames").get_to(frameStep);
        }
    }

    if (!state.exportPalette)
    {
        preserveOrder = true;
        maxColors = is8bpp ? 256 : 16;
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
    hof << "#include \"data/Sprite.hpp\"" << std::endl;
    if (exportAnimation) hof << "#include \"graphics/AnimationPose.hpp\"" << std::endl;
    hof << std::endl;

    auto exportChars = [&](auto charData, const Bitmask& bitmask)
    {
        charData.palette[0] = 0;
        writeCharData(of, state, charData);

        if (generateBitmask) writeBitmaskData(of, bitmask);

        if (!exportAnimation) writeHeaderData(hof, state, charData, bitmask.data.size());
        else
        {
            auto framesX = (charData.chars.width() + state.groupWidth - 1) / state.groupWidth;
            auto framesY = (charData.chars.height() + state.groupHeight - 1) / state.groupHeight;
            writeCharAnimationData(of, state, framesX * framesY);
            writeAnimatedHeaderData(hof, state, charData, animations, frameStep, framesX * framesY, bitmask.data.size());
        } 
    };

    auto image = loadPngToImage(in);
    auto bitmask = generateBitmask ? generateImageBitmask(image) : Bitmask{};
    if (!is8bpp) exportChars(convertImageToCharacters4bpp(image, maxColors, preserveOrder), bitmask);
    else exportChars(convertImageToCharacters8bpp(image, maxColors, preserveOrder), bitmask);

    of.close();
    hof.close();

    return 0;
}

template <typename T>
void writeCharData(std::ostream& of, const State& state, const T& charData)
{
    of << "    .section .rodata." << state.name.mangledName << std::endl;
    of << "    .align 2" << std::endl;
    of << "    .global " << state.name.mangledName << std::endl;
    of << "    .hidden " << state.name.mangledName << std::endl;
    of << state.name.mangledName << ':';
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

    of << std::endl;

    if (state.exportPalette)
    {
        std::size_t index = 0;
        for (Color c : charData.palette)
        {
            if (index % 8 == 0) of << std::endl << "    .hword ";
            else of << ", ";
            of << toHex(c, 4);
            index++;
        }
        of << std::endl;
    }

    of << std::endl;
}

void writeCharAnimationData(std::ofstream& of, const State& state, std::size_t totalNumFrames)
{
    for (std::size_t i = 0; i < totalNumFrames; i++)
        of << "    .hword 0x0000, " << toHex(i, 4) << std::endl;
    of << std::endl;
}

template <typename T>
void writeHeaderData(std::ostream& hof, const State& state, const T& charData, std::size_t bitmaskSize)
{
    hof << "namespace " << state.name.nmspace << std::endl;
    hof << '{' << std::endl;

    std::size_t charSize = charData.chars.width() * charData.chars.height() * charData.chars.at(0, 0).size();
    std::size_t paletteSize = state.exportPalette ? charData.palette.size() : 0;

    hof << "    extern const SpriteHandle<" << charSize << ", " << paletteSize << ", ";
    hof << bitmaskSize << "> " << state.name.fileName << ';' << std::endl;

    hof << '}' << std::endl << std::endl;
}

template <typename T>
void writeAnimatedHeaderData(std::ostream &hof, const State &state, const T& charData,
    const AnimationData &animations, std::size_t frameStep, std::size_t totalNumFrames, std::size_t bitmaskSize)
{
    hof << "namespace " << state.name.nmspace << std::endl;
    hof << '{' << std::endl;

    hof << "    namespace __animation" << std::endl;
    hof << "    {" << std::endl;
    hof << "        struct _ani_" << state.name.fileName << " final" << std::endl;
    hof << "        {" << std::endl;

    for (const auto& p : animations)
    {
        hof << "            constexpr static const AnimationPose " << p.first << " = { ";
        hof << p.second.init << ", " << p.second.repeat << ", " << p.second.end << " };" << std::endl;
    }

    hof << "        };" << std::endl;
    hof << "    }" << std::endl;

    std::size_t charSize = charData.chars.width() * charData.chars.height() * charData.chars.at(0, 0).size();
    std::size_t paletteSize = state.exportPalette ? charData.palette.size() : 0;

    hof << "    extern const SpriteHandle<" << charSize << ", " << paletteSize << ", " << bitmaskSize << ", " << totalNumFrames;
    hof << ", __animation::_ani_" << state.name.fileName << "> " << state.name.fileName << ';' << std::endl;

    hof << '}' << std::endl << std::endl;
}

void writeBitmaskData(std::ostream& of, const Bitmask& bitmask)
{
    of << "    .hword " << toHex(bitmask.hwidth << 7, 4) << ", " << toHex(bitmask.hheight << 7, 4);

    std::size_t index = 0;
    for (std::uint16_t d : bitmask.data)
    {
        if (index % 8 == 0) of << std::endl << "    .hword ";
        else of << ", ";
        of << toHex(d, 4);
        index++;
    }
    of << std::endl;
}
