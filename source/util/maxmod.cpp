//--------------------------------------------------------------------------------
// maxmod.cpp
//--------------------------------------------------------------------------------
// Contains the maxmod initialization 
//--------------------------------------------------------------------------------
#if 0

#include <maxmod.h>
#include <tonc.h>

constexpr u32 NumChannels = 16;

u8 moduleChannels[NumChannels][MM_SIZEOF_MODCH] IWRAM_DATA ALIGN4;
u8 activeChannels[NumChannels][MM_SIZEOF_ACTCH] IWRAM_DATA ALIGN4;
u8 mixingChannels[NumChannels][MM_SIZEOF_MIXCH] IWRAM_DATA ALIGN4;
u8 mixingMemory[MM_MIXLEN_16KHZ] IWRAM_DATA ALIGN4;
u8 waveMemory[MM_MIXLEN_16KHZ] IWRAM_DATA ALIGN4;

extern const u8 soundbank_bin[];

void maxmodInit()
{
    irq_set(II_VBLANK, mmVBlank, ISR_REPLACE);

    mm_gba_system mm;
    mm.mixing_mode = MM_MIX_16KHZ;
    mm.mod_channel_count = NumChannels;
    mm.mix_channel_count = NumChannels;
    mm.module_channels = moduleChannels;
    mm.active_channels = activeChannels;
    mm.mixing_channels = mixingChannels;
    mm.mixing_memory = mixingMemory;
    mm.wave_memory = waveMemory;
    mm.soundbank = (mm_addr)soundbank_bin;

    mmInit(&mm);
}

#endif