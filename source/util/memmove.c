//--------------------------------------------------------------------------------
// memmove.c
//--------------------------------------------------------------------------------
// Provides memmove
//--------------------------------------------------------------------------------
#include <stdint.h>
#include <stddef.h>

void* memcpy(void *dst, const void *src, size_t cnt);

void* memmove(void* dst, const void* src, size_t cnt)
{
    char* dst8 = (char*)dst;
    const char* src8 = (char*)src;

    // Call memcpy if the regions only overlap in the "good sense"
    if (dst8 <= src8 || dst8 >= src8+cnt) return memcpy(dst, src, cnt);

    // Do "chunk calls" to memcpy
    size_t ofs = dst8 - src8;
    dst8 += cnt;
    src8 += cnt;

    while (cnt >= ofs)
    {
        dst8 -= ofs;
        src8 -= ofs;
        cnt -= ofs;
        memcpy(dst8, src8, ofs);
    }

    // Copy the last chunk
    memcpy(dst8-cnt, src8-cnt, cnt);
    return dst;
}
