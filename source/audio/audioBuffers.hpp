//--------------------------------------------------------------------------------
// audioBuffers.hpp
//--------------------------------------------------------------------------------
// Declare the audio mixer's private data
//--------------------------------------------------------------------------------
#pragma once

#include "data/audio-settings.hpp"
#include <tonc.h>

// Declare the mix buffers
extern s8 audioMixBuffersLeft[2][audio::BufferSize];
extern s8 audioMixBuffersRight[2][audio::BufferSize];
extern s8* curAudioMixBufferLeft;
extern s8* curAudioMixBufferRight;
extern u32 curFrame;

// The audio vblank is internal
void audioVblank() IWRAM_CODE;
