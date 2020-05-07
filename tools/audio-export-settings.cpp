#include "pch.hpp"

#include <cmath>

// The allowed frequencies (based on https://deku.gbadev.org/program/sound1.html)
static const std::size_t AllowedFreqs[] = { 5374, 10512, 13379, 18157, 21024, 26758, 31536, 36314, 40137, 42048 };
constexpr static auto AllowedFreqSize = sizeof(AllowedFreqs)/sizeof(AllowedFreqs[0]);

constexpr static std::size_t NumCyclesInVblank = 280896;
constexpr static std::size_t ProcessorClock = 16777216;

int audioExportSettings(int argc, char **argv)
{
    if (argc < 5) throw std::out_of_range("audio-export-settings expects 3 arguments!");

    std::string in = argv[2];
    std::string outh = argv[3];
    std::string outj = argv[4];

    // Load the .json file
    std::ifstream mdin;
    mdin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    mdin.open(in);

    nlohmann::json j;
    mdin >> j;

    auto desiredFreq = j.at("sampling-frequency").get<std::size_t>();
    auto numChannels = j.at("num-channels").get<std::size_t>();

    auto freqPtr = std::lower_bound(AllowedFreqs, AllowedFreqs+AllowedFreqSize, desiredFreq);

    // Now, adjust if necessary
    if (freqPtr == AllowedFreqs+AllowedFreqSize) freqPtr--;
    else if (freqPtr > AllowedFreqs)
        if (freqPtr[0] - desiredFreq >= desiredFreq - freqPtr[-1])
            freqPtr--;

    // Finally, output the data
    // Those formulas here output exactly the same data as the Deku's table
    // (the only difference is for 36314 Hz, where the real timer value is 65074, not 65073)
    auto realFreq = *freqPtr;
    auto step = (std::size_t)std::round((double)ProcessorClock / realFreq);
    auto timerVal = 65536 - step;
    auto bufferSize = NumCyclesInVblank / step;

    nlohmann::json jout;
    jout["sampling-frequency"] = realFreq;

    std::ofstream hof;
    hof.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    hof.open(outh);
    hof << "// " << std::endl;
    hof << "// " << outh << std::endl;
    hof << "// " << std::endl;
    hof << "#pragma once" << std::endl << std::endl;
    hof << "#ifdef __cplusplus" << std::endl;
    hof << "namespace audio" << std::endl;
    hof << '{' << std::endl;
    hof << "    constexpr static auto AudioFrequency = " << realFreq << ';' << std::endl;
    hof << "    constexpr static auto TimerValue = " << timerVal << ';' << std::endl;
    hof << "    constexpr static auto BufferSize = " << bufferSize << ';' << std::endl;
    hof << "    constexpr static auto NumChannels = " << numChannels << ';' << std::endl;
    hof << '}' << std::endl;
    hof << "#else" << std::endl;
    hof << "#define AUDIO_FREQUENCY " << realFreq << std::endl;
    hof << "#define TIMER_VALUE " << timerVal << std::endl;
    hof << "#define BUFFER_SIZE " << bufferSize << std::endl;
    hof << "#define NUM_CHANNELS " << numChannels << std::endl;
    hof << "#endif" << std::endl;
    hof.close();

    std::ofstream jof;
    jof.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    jof.open(outj);
    jof << jout;
    jof.close();

    return 0;
}