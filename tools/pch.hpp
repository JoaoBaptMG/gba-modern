#include <iostream>
#include <fstream>
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cctype>
#include <stdexcept>
#include <algorithm>

#include "grid.hpp"
#include "nlohmann/json.hpp"
#include "util.hpp"

#define LODEPNG_NO_COMPILE_ENCODER
#define LODEPNG_NO_COMPILE_ANCILLARY_CHUNKS
#include "lodepng.h"