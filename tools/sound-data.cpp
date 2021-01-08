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

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <iostream>

inline static std::size_t nextPot(std::size_t n)
{
    if (n == 0) return 0;

    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++;

    return n;
}

float estimateHuffmanSize(const std::vector<int>& val)
{
    std::vector<std::pair<int, int>> histogram;

    {
        // Build the histogram for the vector
        std::unordered_map<int, int> freqs;
        for (int v : val) freqs[v]++;

        histogram.resize(freqs.size());
        std::transform(freqs.begin(), freqs.end(), histogram.begin(),
            [](const auto& p) { return std::make_pair(p.second, p.first); });
        std::sort(histogram.begin(), histogram.end());
    }

    // both >= 0: node, first == -1: leaf, value == second
    std::vector<std::pair<int, int>> nodes;

    {
        std::queue<std::pair<int, int>> leafQueue, nodeQueue;
        for (auto [weight, val] : histogram)
        {
            leafQueue.emplace(weight, nodes.size());
            nodes.emplace_back(-1, val);
        }

        while (leafQueue.size() + nodeQueue.size() > 1)
        {
            int weights[2], indices[2];

            for (std::size_t i = 0; i < 2; i++)
            {
                bool chooseLeaf = nodeQueue.empty() ? true :
                    leafQueue.empty() ? false : (leafQueue.front().first < nodeQueue.front().first);

                auto& queue = chooseLeaf ? leafQueue : nodeQueue;
                std::tie(weights[i], indices[i]) = queue.front();
                queue.pop();
            }

            nodeQueue.emplace(weights[0] + weights[1], nodes.size());
            nodes.emplace_back(indices[0], indices[1]);
        }
    }

    int rootId = nodes.size() - 1;

    // Now, get the bit sizes for everything
    std::unordered_map<int, std::size_t> encodingBitSizes;

    {
        // node id | bit count
        std::queue<std::pair<int, std::size_t>> nodeQueue;
        nodeQueue.emplace(rootId, 0);

        while (!nodeQueue.empty())
        {
            auto [id, count] = nodeQueue.front();
            nodeQueue.pop();

            const auto& pair = nodes[id];
            if (pair.first == -1) // leaf node
                encodingBitSizes[pair.second] = count;
            else
            {
                nodeQueue.emplace(pair.first, count+1);
                nodeQueue.emplace(pair.second, count+1);
            }
        }
    }

    // Now that we have everything, we can test the sizes
    std::size_t totalSum = 0;
    for (int code : val) totalSum += encodingBitSizes[code];

    // Return the possible compression size
    return totalSum / 8.0f;
}

float estimateIndexCodingSize(const std::vector<int>& val)
{
    std::unordered_set<int> values;
    for (int v : val) values.insert(v);

    auto numValues = values.size();
    auto bitSize = 0;
    while (numValues > (1 << bitSize)) bitSize++;

    return numValues + val.size() * bitSize / 8.0f;
}

void testCompression(const std::vector<float>& data)
{
    constexpr auto PacketSize = 352;
    auto numPackets = data.size() / PacketSize;

    float accumMinSize = 0;

    for (std::size_t j = 0; j < numPackets; j++)
    {
        // Here, we will test a compression stuff
        std::vector<std::uint8_t> byteSamples(PacketSize);

        auto ptr = data.data() + j * PacketSize;
        std::transform(ptr, ptr + PacketSize, byteSamples.begin(), floatToUnsigned8bitPcm);

        std::vector<int> verbatim(PacketSize);
        std::copy(byteSamples.begin(), byteSamples.end(), verbatim.begin());

        std::vector<int> constantResidual(PacketSize - 1);
        for (std::size_t i = 0; i < constantResidual.size(); i++)
            constantResidual[i] = (int)byteSamples[i + 1] - byteSamples[i];

        std::vector<int> linearResidual(PacketSize - 2);
        for (std::size_t i = 0; i < linearResidual.size(); i++)
            linearResidual[i] = byteSamples[i + 2] - 2 * (int)byteSamples[i + 1] + byteSamples[i];

        std::vector<int> quadraticResidual(PacketSize - 3);
        for (std::size_t i = 0; i < quadraticResidual.size(); i++)
            quadraticResidual[i] = byteSamples[i + 3] - 3 * byteSamples[i + 2] + 3 * (int)byteSamples[i + 1] - byteSamples[i];

        std::cout << "Packet #" << j << std::endl;

        auto verbatimHuffmanSize = estimateHuffmanSize(verbatim);
        auto constantHuffmanSize = 1 + estimateHuffmanSize(constantResidual);
        auto linearHuffmanSize = 2 + estimateHuffmanSize(linearResidual);
        auto quadraticHuffmanSize = 3 + estimateHuffmanSize(quadraticResidual);

        std::cout << "Total Huffman size for verbatim: " << verbatimHuffmanSize << " bytes ("
                    << (100 * verbatimHuffmanSize / PacketSize) << "% rate)" << std::endl;
        std::cout << "Total Huffman size for constant predictor: " << constantHuffmanSize << " bytes ("
            << (100 * constantHuffmanSize / PacketSize) << "% rate)" << std::endl;
        std::cout << "Total Huffman size for linear predictor: " << linearHuffmanSize << " bytes ("
            << (100 * linearHuffmanSize / PacketSize) << "% rate)" << std::endl;
        std::cout << "Total Huffman size for quadratic predictor: " << quadraticHuffmanSize << " bytes ("
            << (100 * quadraticHuffmanSize / PacketSize) << "% rate)" << std::endl;

        auto constantIndexCodingSize = 1 + estimateIndexCodingSize(constantResidual);
        auto linearIndexCodingSize = 2 + estimateIndexCodingSize(linearResidual);
        auto quadraticIndexCodingSize = 3 + estimateIndexCodingSize(quadraticResidual);

        std::cout << "Total index coding size for constant predictor: " << constantIndexCodingSize << " bytes ("
            << (100 * constantIndexCodingSize / PacketSize) << "% rate)" << std::endl;
        std::cout << "Total index coding size for linear predictor: " << linearIndexCodingSize << " bytes ("
            << (100 * linearIndexCodingSize / PacketSize) << "% rate)" << std::endl;
        std::cout << "Total index coding size for quadratic predictor: " << quadraticIndexCodingSize << " bytes ("
            << (100 * quadraticIndexCodingSize / PacketSize) << "% rate)" << std::endl << std::endl;

        accumMinSize += std::min({ verbatimHuffmanSize, constantHuffmanSize, linearHuffmanSize, quadraticHuffmanSize,
            constantIndexCodingSize, linearIndexCodingSize, quadraticIndexCodingSize, (float)PacketSize });
    }

    std::cout << "Total expected rate: " << (100 * accumMinSize / (numPackets * PacketSize)) << "%" << std::endl;
}
