//--------------------------------------------------------------------------------
// StringBuilder.hpp
//--------------------------------------------------------------------------------
// Class that provides a way to build strings by appending characters, other
// strings and numeric formats
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <tonc.h>

template <typename T>
constexpr std::size_t digitsBase10()
{
    return 0.301029995663981198 * std::numeric_limits<T>::digits + 1;
}

template <std::size_t N>
class StringBuilder final
{
    std::size_t cur;
    char buffer[N+1];

public:
    StringBuilder() : cur(0) { buffer[0] = 0; }

    void append(char ch)
    {
        if (cur < N) buffer[cur++] = ch;
    }

    void append(const char* str)
    {
        for (auto p = str; *p; p++)
        {
            if (cur == N) break;
            buffer[cur++] = *p;
        }
    }

    template <std::size_t M>
    void append(const char str[M])
    {
        std::size_t tsize = std::min(N-cur, M-1);
        std::size_t words = tsize/sizeof(u32);
        std::size_t bytes = words*sizeof(u32);

        if (words > 0) memcpy32(buffer+cur, str, words);
        tsize -= bytes;
        cur += bytes;

        while (tsize--) buffer[cur++] = str[bytes++];
    }

    template <typename T>
    constexpr std::enable_if_t<std::is_unsigned_v<T>>
    append(T v)
    {
        char tmp[digitsBase10<T>()];
        std::size_t i = 0;
        T oldv = v;

        do
        {
            v /= T(10);
            tmp[i++] = oldv - v * T(10);
            oldv = v;
        } while (v);

        while (i-- && cur < N) buffer[cur++] = '0' + tmp[i];
    }

    template <typename T>
    std::enable_if_t<std::is_signed_v<T>>
    append(T v)
    {
        if (v < T(0))
        {
            if (cur < N) buffer[cur++] = '-';
            v = -v;
        }
        append(std::make_unsigned_t<T>(v));
    }

    template <typename... Ts>
    std::enable_if_t<sizeof...(Ts) != 0>
    append(Ts&&... vs)
    {
        ((void)append(std::forward<Ts>(vs)), ...);
    }

    const char* getString()
    { 
        buffer[cur] = 0;
        return buffer;
    }
};
