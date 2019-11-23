//--------------------------------------------------------------------------------
// intmath.h
//--------------------------------------------------------------------------------
// Provides an implementation of some integer math algorithms
//--------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif

#include <tonc.h>
u32 isqrt32(u32 x) IWRAM_CODE;
u32 isqrt64(uint64_t x) IWRAM_CODE;
u32 gcd(u32 a, u32 b);

#ifdef __cplusplus
}
#endif