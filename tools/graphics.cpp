#include <iostream>

#include "graphics.hpp"

static constexpr auto increasingMap()
{
    std::array<std::size_t, 256> map{};
    for (std::size_t i = 0; i < 256; i++) map[i] = i;
    return map;
}

static void throwCodeError(std::size_t maxColors)
{
    throw std::domain_error("Detected pixel with code higher than the maximum permitted (" + std::to_string(maxColors-1) + ")");
}

static auto reducePalette(std::array<Color, 256> &palette, std::size_t maxColors, std::size_t &actualColors)
{
    // Use a map to count the number of distinct palette entries
    std::map<Color, std::size_t> colors;
    for (Color c : palette) colors[c] = 0;

    if (colors.size() > maxColors)
        throw std::invalid_argument("Image must have at most " + std::to_string(maxColors) + " distinct colors!");

    std::size_t index = 1;
    // Special case for the first color in the palette
    for (auto& p : colors)
        if (p.first != palette[0]) p.second = index++;
    actualColors = index;

    std::array<std::size_t, 256> paletteMap;
    std::array<Color, 256> newPalette;
    newPalette.fill(palette.front());

    for (std::size_t i = 0; i < 256; i++)
    {
        paletteMap[i] = colors[palette[i]];
        newPalette[paletteMap[i]] = palette[i];
    }

    palette = std::move(newPalette);

    return paletteMap;
}

CharacterData8bpp convertPngToCharacters8bpp(std::string filename, std::size_t maxColors, bool preserveOrder)
{
    std::vector<std::uint8_t> png;
    std::vector<std::uint8_t> image;
    unsigned int width, height;

    // Configure the raw state
    lodepng::State state;
    state.info_raw.colortype = LCT_PALETTE;
    state.info_raw.bitdepth = 8;

    unsigned int error = lodepng::load_file(png, filename);
    if (!error) error = lodepng::decode(image, width, height, state, png);
    if (error) throw std::domain_error(std::string("Decoder error: ") + lodepng_error_text(error));

    if (width % 8 != 0 || height % 8 != 0)
        throw std::invalid_argument("Image must have width or height multiple of 8!");

    // Assure the image is of the right type
    if (state.info_png.color.colortype != LCT_PALETTE || state.info_png.color.bitdepth > 8)
        throw std::invalid_argument("Image must be a paletted 8bpp (or less) valid PNG!");
    
    CharacterData8bpp charData{};

    // Convert the palette
    std::size_t size = state.info_png.color.palettesize;
    for (std::size_t i = 0; i < size; i++)
    {
        auto r = state.info_png.color.palette[4*i];
        auto g = state.info_png.color.palette[4*i+1];
        auto b = state.info_png.color.palette[4*i+2];
        charData.palette[i] = makeColor(r, g, b);
    }

    // Try to reduce palette
    charData.actualColors = 256;
    auto paletteMap = preserveOrder ? increasingMap() : reducePalette(charData.palette, maxColors, charData.actualColors);

    // Convert to characters
    std::size_t tw = width/8, th = height/8;
    charData.chars.resize(tw, th);

    for (std::size_t y = 0; y < th; y++)
        for (std::size_t x = 0; x < tw; x++)
        {
            // Build the tile
            for (std::size_t j = 0; j < 8; j++)
                for (std::size_t i = 0; i < 8; i++)
                {
                    auto code = image[width * (8*y + j) + (8*x + i)];
                    if (code >= maxColors) throwCodeError(maxColors);
                    charData.chars(x, y)[8*j + i] = paletteMap[code];
                }
        }

    return charData;
}

CharacterData4bpp convertPngToCharacters4bpp(std::string filename, std::size_t maxColors, bool preserveOrder)
{
    auto charData8bpp = convertPngToCharacters8bpp(filename);

    CharacterData4bpp charData{};

    // Try to reduce the palette
    charData.actualColors = 16;
    auto paletteMap = preserveOrder ? increasingMap() : reducePalette(charData8bpp.palette, maxColors, charData.actualColors);

    std::copy_n(charData8bpp.palette.begin(), 16, charData.palette.begin());

    // Now we build the tiles
    std::size_t tw = charData8bpp.chars.width(), th = charData8bpp.chars.height();
    charData.chars.resize(tw, th);

    for (std::size_t y = 0; y < th; y++)
        for (std::size_t x = 0; x < tw; x++)
        {
            // Build the tile
            for (std::size_t j = 0; j < 8; j++)
                for (std::size_t i = 0; i < 4; i++)
                {
                    auto firstCode = charData8bpp.chars(x, y)[8*j + 2*i];
                    auto secondCode = charData8bpp.chars(x, y)[8*j + 2*i+1];

                    if (firstCode >= maxColors) throwCodeError(maxColors);
                    if (secondCode >= maxColors) throwCodeError(maxColors);
                    // This will not overflow
                    charData.chars(x, y)[4*j + i] = paletteMap[firstCode] | (paletteMap[secondCode] << 4);
                }
        }

    return charData;
}

Character4bpp reverseCharacter(Character4bpp ch, Reverse reverse)
{
    if (reverse == Reverse::None) return ch;

    bool h = reverse == Reverse::Horizontal || reverse == Reverse::HorizontalVertical;
    bool v = reverse == Reverse::Vertical || reverse == Reverse::HorizontalVertical;

    Character4bpp rev;

    for (std::size_t j = 0; j < 8; j++)
        for (std::size_t i = 0; i < 4; i++)
        {
            std::size_t tj = v ? 7-j : j;
            if (h)
            {
                auto c = ch[4*tj+(3-i)];
                rev[4*j+i] = ((c << 4) & 0xF0) | ((c >> 4) & 0x0F);
            }
            else rev[4*j+i] = ch[4*tj+i];
        }

    return rev;
}

Character8bpp reverseCharacter(Character8bpp ch, Reverse reverse)
{
    if (reverse == Reverse::None) return ch;

    bool h = reverse == Reverse::Horizontal || reverse == Reverse::HorizontalVertical;
    bool v = reverse == Reverse::Vertical || reverse == Reverse::HorizontalVertical;

    Character8bpp rev;

    for (std::size_t j = 0; j < 8; j++)
        for (std::size_t i = 0; i < 8; i++)
        {
            std::size_t ti = h ? 7-i : i;
            std::size_t tj = v ? 7-j : j;
            rev[8*j+i] = ch[8*tj+ti];
        }

    return rev;
}