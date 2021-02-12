#pragma once

#include <vector>
#include <string>
#include <cstdint>

struct SoundData final
{
    float samplingFreq;
    std::vector<float> samples;
};

SoundData loadWavFile(std::string file);
std::vector<float> resampleMono(const std::vector<float>& samples, float srcRate, float dstRate, float pitchChange = 1.0f);

static inline std::uint8_t floatToUnsigned8bitPcm(float v)
{
    return (int)std::round(v * 127) + 128;
}
