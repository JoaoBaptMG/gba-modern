//--------------------------------------------------------------------------------
// intDigits.h
//--------------------------------------------------------------------------------
// Provides a function to quickly compute the decimal digits of an u32
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

#ifdef __cplusplus
extern "C"
{
#endif
    // Puts in buffer the digits of val, in "crescent order" (least significant
    // bit first), and from 0 to 9 (not from '0' to '9')
    char* uintDigits(char* buffer, u32 val);
#ifdef __cplusplus
}
#endif
