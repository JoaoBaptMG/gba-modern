//--------------------------------------------------------------------------------
// random.h
//--------------------------------------------------------------------------------
// Provides a simple random number generator
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

extern unsigned long long randomState IWRAM_DATA;
u32 random() IWRAM_CODE;