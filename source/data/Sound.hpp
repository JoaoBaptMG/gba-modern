//--------------------------------------------------------------------------------
// Sound.hpp
//--------------------------------------------------------------------------------
// Provides the Sound structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include "math/stdfixed.hpp"

struct Sound
{
    u32 length, loopLength;
    // And after that comes the data in u8 chunks
};

struct SoundHandle
{
    Sound wav;
};
