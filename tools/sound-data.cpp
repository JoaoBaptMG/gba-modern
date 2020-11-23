#include "pch.hpp"

#include "sound-data.hpp"
#include <samplerate.h>

static bool checkMagic(std::ifstream& in, const char* magic)
{
    for (auto p = magic; *p; p++)
        if (in.get() != *p) return false;
    return true;
}

template <typename T>
static T readOne(std::ifstream& in)
{
    T v;
    in.read((char*)&v, sizeof(T));
    return v;
}

SoundData loadWavFile(std::string file)
{
    // Load the WAV file
    std::ifstream in;
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    in.open(file, std::ios::binary);

    if (!checkMagic(in, "RIFF"))
        throw std::domain_error("File " + file + " is not a valid RIFF file!");
    auto chunkSize = readOne<std::uint32_t>(in);
    if (!checkMagic(in, "WAVE"))
        throw std::domain_error("File " + file + " is not a valid wave file!");

    // Read the file format
    if (!checkMagic(in, "fmt "))
        throw std::domain_error("Expected format chunk in file " + file + "!");

    auto fmtSize = readOne<std::uint32_t>(in);
    if (fmtSize != 16)
        throw std::domain_error("Inconsistent format chunk size in file " + file + "!");

    auto format = readOne<std::uint16_t>(in);
    if (format != 1)
        throw std::domain_error("File " + file + " not in PCM format!");

    auto numChannels = readOne<std::uint16_t>(in);
    if (numChannels != 1)
         throw std::domain_error("File " + file + " must have a single channel!");

    auto sourceSampleRate = readOne<std::uint32_t>(in);

    auto byteRate = readOne<std::uint32_t>(in);
    auto blockAlign = readOne<std::uint16_t>(in);
    auto bitsPerSample = readOne<std::uint16_t>(in);

    // Check for data consistency
    if (byteRate != sourceSampleRate * bitsPerSample / 8)
        throw std::domain_error("Inconsistent byte rate for file " + file + "!");
    if (blockAlign != bitsPerSample / 8)
        throw std::domain_error("Inconsistent block align for file " + file + "!");

    if (bitsPerSample != 8 && bitsPerSample != 16)
        throw std::domain_error("Invalid bits per sample value for file " + file + "!");

    // Now, for the data format
    if (!checkMagic(in, "data"))
        throw std::domain_error("Expected data chunk for file " + file + "!");

    auto dataSize = readOne<std::uint32_t>(in);

    // Check for data consistency again
    if (chunkSize != 36 + dataSize)
        throw std::domain_error("Inconsistent size for file " + file + "!");

    SoundData sound;
    sound.samplingFreq = sourceSampleRate;

    auto numSamples = dataSize / (bitsPerSample / 8);
    sound.samples.resize(numSamples);

    if (bitsPerSample == 8)
    {
        std::vector<std::uint8_t> rawSamples(numSamples);
        in.read((char*)rawSamples.data(), numSamples * sizeof(std::uint8_t));
        std::transform(rawSamples.begin(), rawSamples.end(), sound.samples.begin(),
            [] (std::uint8_t v) { return (v - 128) / 127.0f; });
    }
    else
    {
        std::vector<std::int16_t> rawSamples(numSamples);
        in.read((char*)rawSamples.data(), numSamples * sizeof(std::int16_t));
        std::transform(rawSamples.begin(), rawSamples.end(), sound.samples.begin(),
            [] (std::int16_t v) { return std::max(v / 32767.0f, -1.0f); });
    }

    return sound;
}

std::vector<float> resampleMono(const std::vector<float>& samples, float srcRate, float dstRate, float pitchChange)
{
    // A pitch change > than 1 changes the pitch of the sound for a high pitch
    // that means the natural frequencies change by a factor of pitchChange, which means
    // the new sound runs as if the source frequency was multiplied by pitchChange
    double srcRatio = (double)pitchChange * srcRate / dstRate;

    // Deduce a good value for the size of the new vector
    auto size = std::max(samples.size(), std::size_t(2.5 * samples.size() * srcRatio));
    std::vector<float> out(size);

    SRC_DATA src;
    src.data_in = samples.data();
    src.input_frames = samples.size();
    src.data_out = out.data();
    src.output_frames = out.size();
    src.src_ratio = srcRatio;

    int err = src_simple(&src, SRC_SINC_BEST_QUALITY, 1);
    if (err) throw std::runtime_error(std::string("Error happened at libsamplerate: ") + src_strerror(err));

    out.resize(src.output_frames_gen);

    // Normalize the output
    float mv = 0;
    for (float sample : out) mv = std::max(mv, std::abs(sample));
    for (float& sample : out) sample /= mv;

    return out;
}

