//--------------------------------------------------------------------------------
// intDigits.h
//--------------------------------------------------------------------------------
// Provides a function to quickly compute the decimal digits of an u32
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>
#define NOINLINE __attribute__((noinline))

#ifdef __cplusplus
extern "C"
#endif
char* uintDigits(char* buffer, u32 val);
#undef NOINLINE
