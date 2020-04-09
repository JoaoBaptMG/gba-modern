//--------------------------------------------------------------------------------
// intmath.h
//--------------------------------------------------------------------------------
// Provides an implementation of some integer math algorithms
//--------------------------------------------------------------------------------
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <tonc.h>

u32 isqrt32(u32 x) IWRAM_CODE;
u32 isqrt64(uint64_t x) IWRAM_CODE;
u32 gcd(u32 a, u32 b);
u32 ureciprocal32(u32 x) IWRAM_CODE;
s32 sreciprocal32(s32 x) IWRAM_CODE;

#ifdef __cplusplus
}

u32 reciprocal32(u32 x) { return ureciprocal32(x); }
s32 reciprocal32(s32 x) { return sreciprocal32(x); }

#endif