//--------------------------------------------------------------------------------
// audio.hpp
//--------------------------------------------------------------------------------
// The main hub for audio functions
//--------------------------------------------------------------------------------
#pragma once

#include "math/stdfixed.hpp"
#include "data/Sound.hpp"

namespace audio
{
    using Volume = fixed<u32, 4>;
    using PlaySpeed = fixed<u32, 12>;

    void init();

    u32 playSound(const Sound& sound, Volume volume = 1.0, PlaySpeed playSpeed = 1.0);
    void stopSound(u32 channel);

    void mix();
}
