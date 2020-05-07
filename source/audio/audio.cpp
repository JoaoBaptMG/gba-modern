//--------------------------------------------------------------------------------
// audio.hpp
//--------------------------------------------------------------------------------
// The main hub for audio functions
//--------------------------------------------------------------------------------
#include "audio.hpp"

#include "data/audio-settings.hpp"
#include <tonc.h>

#include "util/gba-assert.hpp"
#include "util/profile.hpp"
#include "text/mGBADebugging.hpp"

// Declare the mix buffers
s16 intermediateBuffer[audio::BufferSize];
s8 audioMixBuffers[2][audio::BufferSize];
s8* curAudioMixBuffer;
static u32 curFrame;

// The channel structure
struct AudioChannel
{
    const Sound* sound;
    u32f12 pos, inc;
    u32 volume;
};

AudioChannel audioChannels[audio::NumChannels];

// The audio vblank is internal
static void audioVblank();

void audio::init()
{
    // Use only a single DMA
    REG_DMA1CNT = DMA_DST_FIXED | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;
    REG_DMA1DAD = (u32)&REG_FIFO_A;

    // Set the timer
    REG_TM0D = TimerValue;

    // Set the sound registers: direct sound on mono
    REG_SNDSTAT = SSTAT_ENABLE;
    REG_SNDDSCNT = SDS_DMG100 | SDS_A100 | SDS_AL | SDS_AR | SDS_ATMR0 | SDS_ARESET;

    // Clear all the memory
    curAudioMixBuffer = &audioMixBuffers[0][0];
    curFrame = 0;

    // Install the interrupt
    irq_add(II_VBLANK, audioVblank);
}

void audioVblank()
{
    // Start the timer
    REG_TM0CNT = TM_ENABLE;

    if (curFrame == 0)
    {
        // Restart the DMA
        REG_DMA1CNT = 0;
        REG_DMA1SAD = (u32)&audioMixBuffers[0][0];
        REG_DMA1CNT = DMA_DST_FIXED | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;

        curAudioMixBuffer = &audioMixBuffers[1][0];
        curFrame = 1;
    }
    else
    {
        // Don't need to restart the DMA
        curAudioMixBuffer = &audioMixBuffers[0][0];
        curFrame = 0;
    }
}

u32 audio::playSound(const Sound& sound, u32 volume, u32f12 inc)
{
    int channel;
    for (channel = 0; channel < NumChannels; channel++)
    {
        // Check for an identical channel that just started playing
        if (audioChannels[channel].sound == &sound && audioChannels[channel].pos == 0u)
            return channel;

        if (!audioChannels[channel].sound) break;
    }

    ASSERT(channel < NumChannels);
    if (channel < NumChannels)
    {
        audioChannels[channel].sound = &sound;
        audioChannels[channel].pos = 0;
        audioChannels[channel].inc = inc;
        audioChannels[channel].volume = volume;
    }

    return channel;
}

void audio::stopSound(u32 channel)
{
    audioChannels[channel].sound = nullptr;
}

extern "C" void audioMix(s16* intBuffer, s8* curMixBuffer, AudioChannel* channels) IWRAM_CODE;

void audio::mix()
{
    profile::begin16();
    audioMix(intermediateBuffer, curAudioMixBuffer, audioChannels);
    auto val = profile::end16();

    if (mgba::isEnabled())
        mgba::log(mgba::Log::Debug, "Audio time: ", val, " cycles.");
}
