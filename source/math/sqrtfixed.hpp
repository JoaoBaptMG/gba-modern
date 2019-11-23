//--------------------------------------------------------------------------------
// fixed.hpp
//--------------------------------------------------------------------------------
// Provides the function to get the square root of a fixed point
//--------------------------------------------------------------------------------
#pragma once

#include "fixed.hpp"
#include "intmath.h"

template <typename Ty, std::size_t N, typename = std::enable_if_t<(sizeof(Ty) <= 4)>>
fixed<u32, (N+1)/2> sqrt(fixed<Ty, N> v)
{
    if constexpr (N%2 == 0) return fixed<u32, (N+1)/2>(direct, isqrt32(u32(v.raw())));
    if constexpr (N%2 != 0) return fixed<u32, (N+1)/2>(direct, isqrt32(u32(v.raw()) << 1));
}

template <typename Ty, std::size_t N, typename = std::enable_if_t<(sizeof(Ty) > 4)>>
fixed<u32, (N+1)/2> sqrt(fixed<Ty, N> v)
{
    if constexpr (N%2 == 0) return fixed<u32, (N+1)/2>(direct, isqrt64(v.raw()));
    if constexpr (N%2 != 0) return fixed<u32, (N+1)/2>(direct, isqrt64(v.raw() << 1));
}