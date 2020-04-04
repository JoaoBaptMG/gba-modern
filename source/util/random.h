//--------------------------------------------------------------------------------
// random.h
//--------------------------------------------------------------------------------
// Provides a simple random number generator
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

// It's correct, don't worry
extern unsigned long long randomState;

#ifdef __cplusplus
extern "C"
{
#endif

    u32 IWRAM_CODE random();

#ifdef __cplusplus
}
#endif