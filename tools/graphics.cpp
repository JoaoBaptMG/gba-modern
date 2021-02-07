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

static auto reducePalette(std::array<Color, 256> &palette, std::size_t numColors, std::size_t maxColors,
    std::size_t &actualColors, bool preserveOrder = false)
{
    // Use a map to count the number of distinct palette entries
    std::map<Color, std::size_t> colors;

    std::size_t index = 1;
    for (std::size_t i = 1; i < numColors; i++)
        if (!colors.count(palette[i]))
            colors.emplace(palette[i], index++);

    if (index > maxColors)
        throw std::invalid_argument("Image must have at most " + std::to_string(maxColors) + " distinct colors!");

    actualColors = index;

    if (preserveOrder) return increasingMap();

    std::array<std::size_t, 256> paletteMap;
    std::array<Color, 256> newPalette;
    newPalette.fill(palette.front());

    for (std::size_t i = 1; i < 256; i++)
    {
        paletteMap[i] = colors[palette[i]];
        newPalette[paletteMap[i]] = palette[i];
    }

    palette = std::move(newPalette);

    return paletteMap;
}

Image8bpp loadPngToImage(std::string filename)
{
    std::vector<std::uint8_t> png;
    Image8bpp image;

    // Configure the raw state
    lodepng::State state;
    state.info_raw.colortype = LCT_PALETTE;
    state.info_raw.bitdepth = 8;

    unsigned int error = lodepng::load_file(png, filename);
    if (!error) error = lodepng::decode(image.image, image.width, image.height, state, png);
    if (error) throw std::domain_error(std::string("Decoder error: ") + lodepng_error_text(error));

    // Assure the image is of the right type
    if (state.info_png.color.colortype != LCT_PALETTE || state.info_png.color.bitdepth > 8)
        throw std::invalid_argument("Image must be a paletted 8bpp (or less) valid PNG!");

    std::size_t size = state.info_png.color.palettesize;
    image.palette.resize(size);
    for (std::size_t i = 0; i < size; i++)
    {
        auto r = state.info_png.color.palette[4*i];
        auto g = state.info_png.color.palette[4*i+1];
        auto b = state.info_png.color.palette[4*i+2];
        image.palette[i] = makeColor(r, g, b);
    }

    return image;
}

CharacterData8bpp convertImageToCharacters8bpp(const Image8bpp& image, std::size_t maxColors, bool preserveOrder)
{
    if (image.width % 8 != 0 || image.height % 8 != 0)
        throw std::invalid_argument("Image must have width or height multiple of 8!");
    
    CharacterData8bpp charData{};

    std::copy(image.palette.begin(), image.palette.end(), charData.palette.begin());
    for (std::size_t i = image.palette.size(); i < 256; i++)
        charData.palette[i] = charData.palette[0];

    // Try to reduce palette
    charData.actualColors = 256;
    auto paletteMap = reducePalette(charData.palette, image.palette.size(), maxColors,
        charData.actualColors, preserveOrder);

    // Convert to characters
    std::size_t tw = image.width/8, th = image.height/8;
    charData.chars.resize(tw, th);

    for (std::size_t y = 0; y < th; y++)
        for (std::size_t x = 0; x < tw; x++)
        {
            // Build the tile
            for (std::size_t j = 0; j < 8; j++)
                for (std::size_t i = 0; i < 8; i++)
                {
                    auto code = image(8*x + i, 8*y + j);
                    if (code >= maxColors) throwCodeError(maxColors);
                    charData.chars(x, y)[8*j + i] = paletteMap[code];
                }
        }

    return charData;
}

CharacterData4bpp convertImageToCharacters4bpp(const Image8bpp& image, std::size_t maxColors, bool preserveOrder)
{
    auto charData8bpp = convertImageToCharacters8bpp(image, maxColors, preserveOrder);

    CharacterData4bpp charData{};
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
                    charData.chars(x, y)[4*j + i] = firstCode | (secondCode << 4);
                }
        }

    return charData;
}

Bitmask generateImageBitmask(const Image8bpp& image)
{
    Bitmask bitmask;

    // First, find out the half-width and half-height of the image
    bitmask.hwidth = std::numeric_limits<std::size_t>::min();
    bitmask.hheight = std::numeric_limits<std::size_t>::min();

    auto imagecx = image.width/2;
    auto imagecy = image.height/2;

    for (std::size_t j = 0; j < image.height; j++)
        for (std::size_t i = 0; i < image.width; i++)
            // Check for nonzero outputs
            if (image(i, j) != 0)
            {
                auto newx = i < imagecx ? imagecx-i : i-imagecx;
                auto newy = j < imagecy ? imagecy-j : j-imagecy;

                bitmask.hwidth = std::max(bitmask.hwidth, newx);
                bitmask.hheight = std::max(bitmask.hheight, newy);
            }

    // Now find out the number of hwords needed
    auto clusterSize = (2*bitmask.hwidth + 15)/16;
    auto size = clusterSize * (2*bitmask.hheight);
    bitmask.data.resize(size, 0);

    // And fill in the clusters
    for (std::size_t j = 0; j < 2*bitmask.hheight; j++)
        for (std::size_t i = 0; i < 2*bitmask.hwidth; i++)
        {
            auto x = imagecx - bitmask.hwidth + i;
            auto y = imagecy - bitmask.hheight + j;

            if (image(x, y) != 0)
                bitmask.data[clusterSize*j + i/16] |= 1 << (i%16);
        }

    return bitmask;
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