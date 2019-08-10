#pragma once

#include "defs.hpp"
#include "../tools/grid.hpp"
#include <string>

util::grid<Color> loadImage(const std::string& path, Color* transparentColor = nullptr);
void saveImage(const std::string& path, const util::grid<std::uint32_t>& img);

void blit(const util::grid<Color>& in, std::size_t inX, std::size_t inY,
    util::grid<Color>& out, std::size_t outX, std::size_t outY,
    std::size_t width, std::size_t height);