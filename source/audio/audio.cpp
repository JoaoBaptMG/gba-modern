//--------------------------------------------------------------------------------
// audio.hpp
//--------------------------------------------------------------------------------
// The main hub for audio functions
//--------------------------------------------------------------------------------
#include "audio.hpp"

#include "util/gba-assert.hpp"
#include "util/profile.hpp"
#include "text/mGBADebugging.hpp"

#include "audioBuffers.hpp"

// Materialize the mix buffers
s8 audioMixBuffersLeft[2][audio::BufferSize];
s8 audioMixBuffersRight[2][audio::BufferSize];
s8* curAudioMixBufferLeft;
s8* curAudioMixBufferRight;
u32 curFrame;

// The channel structure
struct AudioChannel
{
    const Sound* sound;
    audio::SpdType pos, playSpeed;
    audio::VolType volumeLeft, volumeRight;
};

AudioChannel audioChannels[audio::NumChannels];

void audio::init()
{
    // Clear all the memory
    memset32(&audioMixBuffersLeft[0][0], 0, audio::BufferSize / sizeof(u32));
    memset32(&audioMixBuffersRight[0][0], 0, audio::BufferSize / sizeof(u32));
    curAudioMixBufferLeft = &audioMixBuffersLeft[0][0];
    curAudioMixBufferRight = &audioMixBuffersRight[0][0];
    curFrame = 0;

    // Set up both DMAs
    REG_DMA1CNT = DMA_DST_FIXED | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;
    REG_DMA1DAD = (u32)&REG_FIFO_A;
    REG_DMA2CNT = DMA_DST_FIXED | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;
    REG_DMA2DAD = (u32)&REG_FIFO_B;

    // Set the timer
    REG_TM0D = TimerValue;

    // Set the sound registers: direct sound on mono
    REG_SNDSTAT = SSTAT_ENABLE;
    REG_SNDDSCNT = SDS_A100 | SDS_B100 | SDS_AL | SDS_BR | SDS_ATMR0 | SDS_BTMR0 | SDS_ARESET | SDS_BRESET;

    // Install the interrupt
    irq_add(II_VBLANK, audioVblank);
}

u32 audio::playSound(const Sound& sound, audio::Volume volume, audio::Pan pan, audio::PlaySpeed playSpeed)
{
    int channel;
    for (channel = 0; channel < NumChannels; channel++)
    {
        // Check for an identical channel that just started playing
        if (audioChannels[channel].sound == &sound && audioChannels[channel].pos == 0u)
            return channel;

        if (!audioChannels[channel].sound) break;
    }

    VolType volumeValue = volume;
    VolType panValue = pan;

    ASSERT(channel < NumChannels);
    if (channel < NumChannels)
    {
        audioChannels[channel].sound = &sound;
        audioChannels[channel].pos = 0;
        audioChannels[channel].volumeLeft = volumeValue * (VolType(1.0) - panValue);
        audioChannels[channel].volumeRight = volumeValue * panValue;
        audioChannels[channel].playSpeed = playSpeed;
    }

    return channel;
}

void audio::stopSound(u32 channel)
{
    audioChannels[channel].sound = nullptr;
}

extern "C" void audioMix(AudioChannel* channels, s8* curMixBufferLeft, s8* curMixBufferRight) IWRAM_CODE;

void audio::mix()
{
    auto numChannels = std::count_if(audioChannels, audioChannels+audio::NumChannels,
        [](const AudioChannel& channel) { return channel.sound; });

    profile::begin32();
    audioMix(audioChannels, curAudioMixBufferLeft, curAudioMixBufferRight);
    auto val = profile::end32();

    if (mgba::isEnabled())
        mgba::log(mgba::Log::Debug, numChannels, " audio channels active; ", val, "/70224 cycles.");
}
