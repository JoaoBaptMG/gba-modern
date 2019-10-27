//--------------------------------------------------------------------------------
// AnimationData.hpp
//--------------------------------------------------------------------------------
// Provide a structure that has info on a single animation pose
//--------------------------------------------------------------------------------

#pragma once

#include <cstdint>

struct AnimationPose
{
    std::uint16_t init, repeat, end;
};
