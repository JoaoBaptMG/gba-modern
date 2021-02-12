//--------------------------------------------------------------------------------
// audio.hpp
//--------------------------------------------------------------------------------
// The main hub for audio functions
//--------------------------------------------------------------------------------
#pragma once

#include "math/stdfixed.hpp"
#include "data/Sound.hpp"
#include "util/ExplicitTypedef.hpp"

namespace audio
{
    using VolType = fixed<u32, 4>;
    using SpdType = fixed<u32, 12>;

    EXPLICIT_TYPEDEF(Volume, VolType);
    EXPLICIT_TYPEDEF(Pan, VolType);
    EXPLICIT_TYPEDEF(PlaySpeed, SpdType);

    constexpr auto DefaultVolume = Volume(1.0);
    constexpr auto DefaultPan = Pan(0.5);
    constexpr auto DefaultPlaySpeed = PlaySpeed(1.0);

    void init();

    u32 playSound(const Sound& sound, Volume volume = DefaultVolume,
        Pan pan = DefaultPan, PlaySpeed playSpeed = DefaultPlaySpeed);

    // All combinatorial possibilities
    inline static u32 playSound(const Sound& sound, Pan pan, Volume volume = DefaultVolume,
        PlaySpeed playSpeed = DefaultPlaySpeed) { return playSound(sound, volume, pan, playSpeed); }

    inline static u32 playSound(const Sound& sound, PlaySpeed playSpeed, Volume volume = DefaultVolume,
        Pan pan = DefaultPan) { return playSound(sound, volume, pan, playSpeed); }

    inline static u32 playSound(const Sound& sound, Volume volume, PlaySpeed playSpeed, Pan pan = DefaultPan)
    { return playSound(sound, volume, pan, playSpeed); }

    inline static u32 playSound(const Sound& sound, Pan pan, PlaySpeed playSpeed, Volume volume = DefaultVolume)
    { return playSound(sound, volume, pan, playSpeed); }

    inline static u32 playSound(const Sound& sound, PlaySpeed playSpeed, Pan pan, Volume volume = DefaultVolume)
    { return playSound(sound, volume, pan, playSpeed); }

    void stopSound(u32 channel);

    void mix();
}
