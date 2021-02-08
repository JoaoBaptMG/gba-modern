//--------------------------------------------------------------------------------
// allocator.hpp
//--------------------------------------------------------------------------------
// Provides a routine for load overlays from IWRAM. They are mutually exclusive,
// not being able to "allocate" them on IWRAM though
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

extern const char __load_start_iwram0[], __load_stop_iwram0[];
extern const char __load_start_iwram1[], __load_stop_iwram1[];
extern const char __load_start_iwram2[], __load_stop_iwram2[];
extern const char __load_start_iwram3[], __load_stop_iwram3[];
extern const char __load_start_iwram4[], __load_stop_iwram4[];
extern const char __load_start_iwram5[], __load_stop_iwram5[];
extern const char __load_start_iwram6[], __load_stop_iwram6[];
extern const char __load_start_iwram7[], __load_stop_iwram7[];
extern const char __load_start_iwram8[], __load_stop_iwram8[];
extern const char __load_start_iwram9[], __load_stop_iwram9[];
extern char __iwram_overlay_start[];

#define OVERLAY_LOAD(id) memcpy32(__iwram_overlay_start, __load_start_iwram##id,\
    (__load_stop_iwram##id - __load_start_iwram##id) / sizeof(u32))
