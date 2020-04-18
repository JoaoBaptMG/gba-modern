//--------------------------------------------------------------------------------
// aslatin1.hpp
//--------------------------------------------------------------------------------
// Add a user defined literal that converts Unicode characters to Latin-1
// (ASCII + the Latin-1 supplement) in a single byte
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <type_traits>
#include <initializer_list>
#include <array>

template <std::size_t Size>
struct Latin1String
{
    std::array<T, Size+1> chars;
    constexpr operator const char*() const { return chars.data(); }
};

template <typename T, T... Chars>
constexpr auto operator ""_aslatin1()
{
    constexpr auto Size = (std::size_t(Chars <= 128 || Chars > 192) + ...);
    std::array<T, Size+1> chars{};
    std::size_t i = 0;

    using UT = std::make_unsigned_t<T>;
    UT b = 0;
    for (UT c : { Chars... })
    {
        if (c < 128) chars[i++] = c;
        else if (c < 192) chars[i++] = ((b & 0x1F) << 6) | (c & 0x3F);
        else b = c;
    }
    chars[Size] = 0;

    return Latin1String<Size>{chars};
}
