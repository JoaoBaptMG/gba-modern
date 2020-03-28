//--------------------------------------------------------------------------------
// stdfixed.hpp
//--------------------------------------------------------------------------------
// Provides standard fixed point libraries
//--------------------------------------------------------------------------------
#pragma once

#include "fixed.hpp"
#include "tonc.h"

#include <cstdint>

// Fixed point formats for unsigned types
template <std::size_t N> using u8f = fixed<u8, N>;
template <std::size_t N> using u16f = fixed<u16, N>;
template <std::size_t N> using u32f = fixed<u32, N>;
template <std::size_t N> using u64f = fixed<std::uint64_t, N>;

// Fixed point formats for signed types
template <std::size_t N> using s8f = fixed<s8, N>;
template <std::size_t N> using s16f = fixed<s16, N>;
template <std::size_t N> using s32f = fixed<s32, N>;
template <std::size_t N> using s64f = fixed<std::int64_t, N>;

// Commonly used unsigned fixed point types
using u16f8 = u16f<8>;
using u32f8 = u32f<8>;
using u32f16 = u32f<16>;
using u64f8 = u64f<8>;
using u64f16 = u64f<16>;
using u64f32 = u64f<32>;

// Commonly used signed fixed point types
using s16f8 = s16f<8>;
using s32f8 = s32f<8>;
using s32f16 = s32f<16>;
using s64f8 = s64f<8>;
using s64f16 = s64f<16>;
using s64f32 = s64f<32>;
