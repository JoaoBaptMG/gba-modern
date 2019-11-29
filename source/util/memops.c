//--------------------------------------------------------------------------------
// memops.c
//--------------------------------------------------------------------------------
// Provides an optimized version of memcpy, memset and memmove
//--------------------------------------------------------------------------------
#include <stdint.h>
#include <tonc.h>

void* memcpy(void* dst, const void* src, size_t cnt)
{
    // Return early
    if (dst == src) return dst;

    unsigned char* dst8 = (unsigned char*)dst;
    const unsigned char* src8 = (const unsigned char*)src;

    // The alignment
    uint32_t align = ((uint32_t)dst ^ (uint32_t)src) & 3;

    // If the number of bytes is enough and the addresses possibly have the same alignment, copy them using memset32
    if (cnt > 16 && align == 0)
    {
        // Align them first
        while ((uint32_t)dst8 & 3)
        {
            *dst8++ = *src8++;
            cnt--;
        }
        
        // Copy using memset32 and adjust parameters
        memcpy32(dst8, src8, cnt/4);
        dst8 += cnt & ~3;
        src8 += cnt & ~3;
        cnt &= ~3;
    }
    else if (align == 2) // Same thing, copying halfwords at a time
    {
        uint16_t* dst16 = (uint16_t*)dst;
        const uint16_t* src16 = (const uint16_t*)src;

        while (cnt & ~1)
        {
            *dst16++ = *src16++;
            cnt -= 2;
        }
    }

    while (cnt--) *dst8 = *src8;

    return dst;
}

void* memset(void* dst, int val, size_t cnt)
{
    unsigned char* dst8 = (unsigned char*)dst;

    // Truncate val to 8 bits
    val &= 255;

    // Align first
    while ((uint32_t)dst8 & 3)
    {
        *dst8++ = val;
        cnt--;
    }

    // Now do a memset32 and align the ptrs
    int val32 = val | (val << 8);
    val32 |= val32 << 16;
    memset32(dst8, val32, cnt/4);
    dst8 += cnt & ~3;
    cnt &= 3;

    // Copy the last bytes
    while (cnt--) *dst8++ = val;

    return dst;
}

void* memmove(void* dst, const void* src, size_t cnt)
{
    unsigned char* dst8 = (unsigned char*)dst;
    const unsigned char* src8 = (const unsigned char*)src;

    // Call memcpy if the regions only overlap in the "good sense"
    if (dst8 <= src8 || dst8 >= src8+cnt) return memcpy(dst, src, cnt);

    // Do a very dumb backwards copy
    dst8 += cnt;
    src8 += cnt;
    while (cnt--) *dst8-- = *src8--;

    return dst;
}
