//--------------------------------------------------------------------------------
// audioBuffers.hpp
//--------------------------------------------------------------------------------
// Declare the audio mixer's private data
//--------------------------------------------------------------------------------
#pragma once

#include "data/audio-settings.hpp"
#include <tonc.h>

// Declare the mix buffers
extern s16 intermediateBuffer[audio::BufferSize];
extern s8 audioMixBuffers[2][audio::BufferSize];
extern s8* curAudioMixBuffer;
extern u32 curFrame;

// The audio vblank is internal
void audioVblank() IWRAM_CODE;
