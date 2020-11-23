#include "pch.hpp"

#include "sound-data.hpp"

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
    auto [sourceSampleRate, soundData] = loadWavFile(in);

    if (sourceSampleRate != samplingFreq)
        soundData = resampleMono(soundData, sourceSampleRate, samplingFreq);

    // Pad the sound data to 4 bytes
    soundData.resize((soundData.size() + 3) & ~3, 0.0f);

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

        of << "    .word " << soundData.size() << ", 0";

        std::size_t index = 0;
        for (float& sample : soundData)
        {
            if (index % 16 == 0) of << std::endl << "    .byte ";
            else of << ", ";
            of << toHex(floatToUnsigned8bitPcm(sample), 2);
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
