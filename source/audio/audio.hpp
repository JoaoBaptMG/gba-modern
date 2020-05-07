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
    constexpr u32 MaxVolume = 64;

    void init();

    u32 playSound(const Sound& sound, u32 volume = MaxVolume, u32f12 inc = u32f12(1.0));
    void stopSound(u32 channel);

    void mix();
}
