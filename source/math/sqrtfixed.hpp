//--------------------------------------------------------------------------------
// fixed.hpp
//--------------------------------------------------------------------------------
// Provides the function to get the square root of a fixed point
//--------------------------------------------------------------------------------
#pragma once

#include "fixed.hpp"
#include "intmath.h"

template <std::size_t N>
fixed<u32, (N+1)/2> sqrt(fixed<u32, N> v)
{
    if constexpr (N%2 == 0) return fixed<u32, (N+1)/2>(direct, isqrt32(v.raw()));
    if constexpr (N%2 != 0) return fixed<u32, (N+1)/2>(direct, isqrt32(v.raw() << 1));
}
