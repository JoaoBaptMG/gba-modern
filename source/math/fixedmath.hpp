//--------------------------------------------------------------------------------
// fixed.hpp
//--------------------------------------------------------------------------------
// Provides the function to get the square root of a fixed point
//--------------------------------------------------------------------------------
#pragma once

#include "fixed.hpp"
#include "intmath.h"

template <typename T>
using signed32 = std::conditional_t<std::is_signed_v<T>, s32, u32>;

template <typename Ty, std::size_t N, typename = std::enable_if_t<(sizeof(Ty) <= 4)>>
fixed<Ty, (N+1)/2> sqrt(fixed<Ty, N> v)
{
    if constexpr (N%2 == 0) return fixed<Ty, (N+1)/2>(direct, isqrt32(u32(v.raw())));
    if constexpr (N%2 != 0) return fixed<Ty, (N+1)/2>(direct, isqrt32(u32(v.raw()) << 1));
}

template <typename Ty, std::size_t N, typename = std::enable_if_t<(sizeof(Ty) > 4)>>
fixed<signed32<Ty>, (N+1)/2> sqrt(fixed<Ty, N> v)
{
    if constexpr (N%2 == 0) return fixed<signed32<Ty>, (N+1)/2>(direct, isqrt64(v.raw()));
    if constexpr (N%2 != 0) return fixed<signed32<Ty>, (N+1)/2>(direct, isqrt64(v.raw() << 1));
}

template <typename Ty, std::size_t N>
fixed<Ty, 32-N> reciprocal(fixed<Ty, N> v)
{
    return fixed<Ty, 32-N>(direct, reciprocal32(signed32<Ty>(v.raw())));
}
