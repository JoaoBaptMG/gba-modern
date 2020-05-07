//--------------------------------------------------------------------------------
// Sound.hpp
//--------------------------------------------------------------------------------
// Provides the Sound structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include "math/stdfixed.hpp"

struct Sound
{
    u32f12 length, loopLength;
    // And after that comes the data in s8 chunks
};

struct SoundHandle
{
    Sound wav;
};
