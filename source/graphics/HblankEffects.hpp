//--------------------------------------------------------------------------------
// HblankEffects.hpp
//--------------------------------------------------------------------------------
// Provides the class to manage the H-Blank effects
//--------------------------------------------------------------------------------
#pragma once

#include <tonc.h>

class HblankEffects final
{
    int makeSpaceForEffect(int line);

public:
    void init();
    void vblank();
    void flush();

    void add16(int line, const void* src, void* dst, u32 nhwords);
    void add32(int line, const void* src, void* dst, u32 nwords);
};
