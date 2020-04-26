//--------------------------------------------------------------------------------
// profile.cpp
//--------------------------------------------------------------------------------
// Profiling functions
//--------------------------------------------------------------------------------
#include "profile.hpp"

#include <tonc.h>

void profile::begin16(Precision precision)
{
    REG_TM2D = 0;
    REG_TM2CNT = 0;
    REG_TM2CNT = static_cast<u32>(precision) | TM_ENABLE;
    asm volatile ("" ::: "memory");
}

void profile::begin32(Precision precision)
{
    REG_TM2D = 0; REG_TM3D = 0;
    REG_TM2CNT = 0; REG_TM3CNT = 0;
    REG_TM3CNT = TM_CASCADE | TM_ENABLE;
    REG_TM2CNT = static_cast<u32>(precision) | TM_ENABLE;
    asm volatile ("" ::: "memory");
}

std::uint16_t profile::end16()
{
    asm volatile ("" ::: "memory");
    REG_TM2CNT = 0;
    return REG_TM2D;
}

std::uint16_t profile::end32()
{
    asm volatile ("" ::: "memory");
    REG_TM2CNT = 0;
    return ((u32)REG_TM3D << 16) | REG_TM2D;
}