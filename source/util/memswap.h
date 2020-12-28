//--------------------------------------------------------------------------------
// memswap32.h
//--------------------------------------------------------------------------------
// Provides a function to swap two chunks of memory
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

#ifdef __cplusplus
extern "C"
#endif
void memswap32(void* mem1, void* mem2, u32 nwords) IWRAM_CODE;
