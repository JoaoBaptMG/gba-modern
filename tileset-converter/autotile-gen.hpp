#pragma once

#include "defs.hpp"
#include "../tools/grid.hpp"

void expandAutotile(const std::string &baseName, std::size_t id, const util::grid<Color>& in);
void generateTileset(const std::string &baseName, std::size_t id, Color *transparentColor = nullptr);
void generateRule(const std::string &baseName, std::size_t id);