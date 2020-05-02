//--------------------------------------------------------------------------------
// BitmaskData.hpp
//--------------------------------------------------------------------------------
// Provides the BitmaskData structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

struct BitmaskData { vec2<s16f7> halfSize; };
// The bitmask data is only used in ARM assembly, so it is not a big deal.
// Essentially, the first 4 bytes are a vec2<s16f7> representing the bitmask's
// bounding box. Let pixelHeight = 2*floor(bbox.y) and clusterSize = ceil(bbox.x/16)
// We will have clusterSize hwords forming a cluster that represents the bitmask,
// from left to right, and there are pixelHeight clusters from top to bottom.
// The bitmask is aligned to the top-left of the bounding box.
