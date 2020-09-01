#pragma once

#include "pch.hpp"

std::vector<float> resampleMono(const std::vector<float>& samples, float srcRate, float dstRate, float pitchChange = 1.0f);
