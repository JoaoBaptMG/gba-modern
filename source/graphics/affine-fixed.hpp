//--------------------------------------------------------------------------------
// affine-fixed.hpp
//--------------------------------------------------------------------------------
// Provids aliases for the affine registers in fixed point form
//--------------------------------------------------------------------------------
#pragma once

#include "math/stdfixed.hpp"

#define REG_BG2PA_FX *(volatile s16f8*)(0x4000020)
#define REG_BG2PB_FX *(volatile s16f8*)(0x4000022)
#define REG_BG2PC_FX *(volatile s16f8*)(0x4000024)
#define REG_BG2PD_FX *(volatile s16f8*)(0x4000026)
#define REG_BG2X_FX *(volatile s32f8*)(0x4000028)
#define REG_BG2Y_FX *(volatile s32f8*)(0x400002C)

#define REG_BG3PA_FX *(volatile s16f8*)(0x4000030)
#define REG_BG3PB_FX *(volatile s16f8*)(0x4000032)
#define REG_BG3PC_FX *(volatile s16f8*)(0x4000034)
#define REG_BG3PD_FX *(volatile s16f8*)(0x4000036)
#define REG_BG3X_FX *(volatile s32f8*)(0x4000038)
#define REG_BG3Y_FX *(volatile s32f8*)(0x400003C)
