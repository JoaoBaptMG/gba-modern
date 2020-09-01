#include "resample.hpp"
#include <samplerate.h>

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
    float mv = *std::max_element(out.begin(), out.end());
    for (float& sample : out) sample /= mv;

    return out;
}
