//--------------------------------------------------------------------------------
// audio.hpp
//--------------------------------------------------------------------------------
// The main hub for audio functions
//--------------------------------------------------------------------------------
#pragma once

#include "../math/stdfixed.hpp"
#include "../data/Sound.hpp"

namespace audio
{
    constexpr u32 MaxVolume = 16;

    void init();

    u32 playSound(const Sound& sound, u32 volume = MaxVolume);
    void stopSound(u32 channel);

    void mix();
}
