#include "tgraphics.hpp"
#include <stdexcept>
#include "../tools/lodepng.h"

util::grid<Color> loadImage(const std::string& path, Color* transparentColor)
{
    lodepng::State state;
    state.info_raw.colortype = LCT_RGBA;
    state.info_raw.bitdepth = 8;

    std::vector<unsigned char> png;
    std::vector<unsigned char> data;
    unsigned int w, h;
    int error = lodepng::load_file(png, path);
    if (!error) error = lodepng::decode(data, w, h, state, png);
    if (error) throw std::runtime_error(lodepng_error_text(error));


    if (transparentColor)
    {
        std::size_t r = state.info_png.color.palette[0];
        std::size_t g = state.info_png.color.palette[1];
        std::size_t b = state.info_png.color.palette[2];
        std::size_t a = state.info_png.color.palette[3];
        *transparentColor = (r << 24) | (g << 16) | (b << 8) | a;
    }

    return util::grid<Color>(w, h, reinterpret_cast<Color*>(data.data()));
}

void saveImage(const std::string& path, const util::grid<std::uint32_t>& img)
{
    int error = lodepng::encode(path,
        reinterpret_cast<const unsigned char*>(img.data()),
        img.width(), img.height());
    if (error) throw std::runtime_error(lodepng_error_text(error));
}

void blit(const util::grid<Color>& in, std::size_t inX, std::size_t inY,
    util::grid<std::uint32_t>& out, std::size_t outX, std::size_t outY,
    std::size_t width, std::size_t height)
{
    for (std::size_t j = 0; j < height; j++)
        std::copy_n(&in(inX, inY+j), width, &out(outX, outY+j));
}