#include "pch.hpp"

bool checkMagic(std::ifstream& in, const char* magic)
{
    for (auto p = magic; *p; p++)
        if (in.get() != *p) return false;
    return true;
}

template <typename T>
T readFrom(std::ifstream& in)
{
    T v;
    in.read((char*)&v, sizeof(T));
    return v;
}

int soundExport(int argc, char  **argv)
{
    if (argc < 6) throw std::out_of_range("sound-export expects 4 arguments!");

    std::string in = argv[2];
    std::string out = argv[3];
    std::string outh = argv[4];
    std::string ins = argv[5];

    // Load general audio settings here
    nlohmann::json j;
    {
        std::ifstream sets;
        sets.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        sets.open(ins);
        sets >> j;
    }

    auto samplingFreq = j.at("sampling-frequency").get<std::size_t>();

    // Load the WAV file
    std::ifstream win;
    win.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    win.open(in);

    if (!checkMagic(win, "RIFF"))
        throw std::domain_error("File " + in + " is not a valid RIFF file!");
    auto chunkSize = readFrom<std::uint32_t>(win);
    if (!checkMagic(win, "WAVE"))
        throw std::domain_error("File " + in + " is not a valid wave file!");

    // Read the file format
    if (!checkMagic(win, "fmt "))
        throw std::domain_error("Expected format chunk in file " + in + "!");

    auto fmtSize = readFrom<std::uint32_t>(win);
    if (fmtSize != 16)
        throw std::domain_error("Inconsistent format chunk size in file " + in + "!");

    auto format = readFrom<std::uint16_t>(win);
    if (format != 1)
        throw std::domain_error("File " + in + " not in PCM format!");

    auto numChannels = readFrom<std::uint16_t>(win);
    if (numChannels != 1)
         throw std::domain_error("File " + in + " must have a single channel!");

    auto sourceSampleRate = readFrom<std::uint32_t>(win);
    if (sourceSampleRate != samplingFreq)
        throw std::domain_error("File " + in + " must have a matching sampling rate (" + std::to_string(samplingFreq) + " Hz)!");

    auto byteRate = readFrom<std::uint32_t>(win);
    auto blockAlign = readFrom<std::uint16_t>(win);
    auto bitsPerSample = readFrom<std::uint16_t>(win);

    // Check for data consistency
    if (byteRate != sourceSampleRate * bitsPerSample / 8)
        throw std::domain_error("Inconsistent byte rate for file " + in + "!");
    if (blockAlign != bitsPerSample / 8)
        throw std::domain_error("Inconsistent block align for file " + in + "!");

    if (bitsPerSample != 8 && bitsPerSample != 16)
        throw std::domain_error("Invalid bits per sample value for file " + in + "!");

    // Now, for the data format
    if (!checkMagic(win, "data"))
        throw std::domain_error("Expected data chunk for file " + in + "!");

    auto dataSize = readFrom<std::uint32_t>(win);

    // Check for data consistency again
    if (chunkSize != 36 + dataSize)
        throw std::domain_error("Inconsistent size for file " + in + "!");

    auto numSamples = dataSize / (bitsPerSample / 8);
    std::vector<int> originalData(numSamples);

    for (int& val : originalData)
    {
        if (bitsPerSample == 8)
            val = readFrom<std::uint8_t>(win) - 128;
        else if (bitsPerSample == 16)
            val = readFrom<std::int16_t>(win) / 256;
    }

    auto name = deriveSpecialName(in);

    {
        std::ofstream of;
        of.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        of.open(out);
        of << "@ " << std::endl;
        of << "@ " << out << std::endl;
        of << "@ " << std::endl << std::endl;

        of << "    .section .rodata." << name.mangledName << std::endl;
        of << "    .align 2" << std::endl;
        of << "    .global " << name.mangledName << std::endl;
        of << "    .hidden " << name.mangledName << std::endl;
        of << name.mangledName << ':' << std::endl;

        of << "    .word " << toHex(numSamples << 12) << ", 0";

        std::size_t index = 0;
        for (int& v : originalData)
        {
            if (index % 16 == 0) of << std::endl << "    .byte ";
            else of << ", ";
            of << toHex(v & 0xFF, 2);
            index++;
        }

        of << std::endl;
        of.close();
    }

    {
        std::ofstream hof;
        hof.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        hof.open(outh);

        hof << "// " << std::endl;
        hof << "// " << outh << std::endl;
        hof << "// " << std::endl;
        hof << "#pragma once" << std::endl << std::endl;
        hof << "#include \"data/Sound.hpp\"" << std::endl << std::endl;
        hof << "namespace " << name.nmspace << std::endl;
        hof << '{' << std::endl;
        hof << "    extern const SoundHandle " << name.fileName << ';' << std::endl;
        hof << '}' << std::endl;
        hof.close();
    }

    return 0;
}
