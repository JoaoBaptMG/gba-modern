//--------------------------------------------------------------------------------
// colors.hpp
//--------------------------------------------------------------------------------
// Contains a list of colors
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>

namespace colors
{
    namespace detail
    {
        struct Color { std::uint8_t r, g, b; };

        inline static constexpr std::uint16_t color(std::uint8_t r, std::uint8_t g, std::uint8_t b)
        {
            return r | (g << 5) | (b << 10);
        }
    }

    constexpr auto AliceBlue = detail::color(30, 31, 31);
    constexpr auto AntiqueWhite = detail::color(31, 29, 26);
    constexpr auto Aqua = detail::color(0, 31, 31);
    constexpr auto Aquamarine = detail::color(15, 31, 26);
    constexpr auto Azure = detail::color(30, 31, 31);
    constexpr auto Beige = detail::color(30, 30, 27);
    constexpr auto Bisque = detail::color(31, 28, 24);
    constexpr auto Black = detail::color(0, 0, 0);
    constexpr auto BlanchedAlmond = detail::color(31, 29, 25);
    constexpr auto Blue = detail::color(0, 0, 31);
    constexpr auto BlueViolet = detail::color(17, 5, 28);
    constexpr auto Brown = detail::color(20, 5, 5);
    constexpr auto BurlyWood = detail::color(27, 23, 16);
    constexpr auto CadetBlue = detail::color(11, 19, 20);
    constexpr auto Chartreuse = detail::color(15, 31, 0);
    constexpr auto Chocolate = detail::color(26, 13, 3);
    constexpr auto Coral = detail::color(31, 15, 10);
    constexpr auto CornflowerBlue = detail::color(12, 18, 29);
    constexpr auto Cornsilk = detail::color(31, 31, 27);
    constexpr auto Crimson = detail::color(27, 2, 7);
    constexpr auto Cyan = detail::color(0, 31, 31);
    constexpr auto DarkBlue = detail::color(0, 0, 17);
    constexpr auto DarkCyan = detail::color(0, 17, 17);
    constexpr auto DarkGoldenRod = detail::color(23, 16, 1);
    constexpr auto DarkGray = detail::color(21, 21, 21);
    constexpr auto DarkGrey = detail::color(21, 21, 21);
    constexpr auto DarkGreen = detail::color(0, 12, 0);
    constexpr auto DarkKhaki = detail::color(23, 22, 13);
    constexpr auto DarkMagenta = detail::color(17, 0, 17);
    constexpr auto DarkOliveGreen = detail::color(10, 13, 5);
    constexpr auto DarkOrange = detail::color(31, 17, 0);
    constexpr auto DarkOrchid = detail::color(19, 6, 25);
    constexpr auto DarkRed = detail::color(17, 0, 0);
    constexpr auto DarkSalmon = detail::color(29, 18, 15);
    constexpr auto DarkSeaGreen = detail::color(17, 23, 17);
    constexpr auto DarkSlateBlue = detail::color(9, 7, 17);
    constexpr auto DarkSlateGray = detail::color(5, 9, 9);
    constexpr auto DarkSlateGrey = detail::color(5, 9, 9);
    constexpr auto DarkTurquoise = detail::color(0, 25, 26);
    constexpr auto DarkViolet = detail::color(18, 0, 26);
    constexpr auto DeepPink = detail::color(31, 2, 18);
    constexpr auto DeepSkyBlue = detail::color(0, 23, 31);
    constexpr auto DimGray = detail::color(13, 13, 13);
    constexpr auto DimGrey = detail::color(13, 13, 13);
    constexpr auto DodgerBlue = detail::color(3, 18, 31);
    constexpr auto FireBrick = detail::color(22, 4, 4);
    constexpr auto FloralWhite = detail::color(31, 31, 30);
    constexpr auto ForestGreen = detail::color(4, 17, 4);
    constexpr auto Fuchsia = detail::color(31, 0, 31);
    constexpr auto Gainsboro = detail::color(27, 27, 27);
    constexpr auto GhostWhite = detail::color(31, 31, 31);
    constexpr auto Gold = detail::color(31, 26, 0);
    constexpr auto GoldenRod = detail::color(27, 20, 4);
    constexpr auto Gray = detail::color(16, 16, 16);
    constexpr auto Grey = detail::color(16, 16, 16);
    constexpr auto Green = detail::color(0, 16, 0);
    constexpr auto GreenYellow = detail::color(21, 31, 5);
    constexpr auto HoneyDew = detail::color(30, 31, 30);
    constexpr auto HotPink = detail::color(31, 13, 22);
    constexpr auto IndianRed = detail::color(25, 11, 11);
    constexpr auto Indigo = detail::color(9, 0, 16);
    constexpr auto Ivory = detail::color(31, 31, 30);
    constexpr auto Khaki = detail::color(30, 28, 17);
    constexpr auto Lavender = detail::color(28, 28, 31);
    constexpr auto LavenderBlush = detail::color(31, 30, 30);
    constexpr auto LawnGreen = detail::color(15, 31, 0);
    constexpr auto LemonChiffon = detail::color(31, 31, 25);
    constexpr auto LightBlue = detail::color(21, 27, 28);
    constexpr auto LightCoral = detail::color(30, 16, 16);
    constexpr auto LightCyan = detail::color(28, 31, 31);
    constexpr auto LightGoldenRodYellow = detail::color(31, 31, 26);
    constexpr auto LightGray = detail::color(26, 26, 26);
    constexpr auto LightGrey = detail::color(26, 26, 26);
    constexpr auto LightGreen = detail::color(18, 29, 18);
    constexpr auto LightPink = detail::color(31, 22, 24);
    constexpr auto LightSalmon = detail::color(31, 20, 15);
    constexpr auto LightSeaGreen = detail::color(4, 22, 21);
    constexpr auto LightSkyBlue = detail::color(16, 25, 31);
    constexpr auto LightSlateGray = detail::color(14, 17, 19);
    constexpr auto LightSlateGrey = detail::color(14, 17, 19);
    constexpr auto LightSteelBlue = detail::color(22, 24, 27);
    constexpr auto LightYellow = detail::color(31, 31, 28);
    constexpr auto Lime = detail::color(0, 31, 0);
    constexpr auto LimeGreen = detail::color(6, 25, 6);
    constexpr auto Linen = detail::color(31, 30, 28);
    constexpr auto Magenta = detail::color(31, 0, 31);
    constexpr auto Maroon = detail::color(16, 0, 0);
    constexpr auto MediumAquaMarine = detail::color(12, 25, 21);
    constexpr auto MediumBlue = detail::color(0, 0, 25);
    constexpr auto MediumOrchid = detail::color(23, 10, 26);
    constexpr auto MediumPurple = detail::color(18, 14, 27);
    constexpr auto MediumSeaGreen = detail::color(7, 22, 14);
    constexpr auto MediumSlateBlue = detail::color(15, 13, 29);
    constexpr auto MediumSpringGreen = detail::color(0, 31, 19);
    constexpr auto MediumTurquoise = detail::color(9, 26, 25);
    constexpr auto MediumVioletRed = detail::color(24, 2, 16);
    constexpr auto MidnightBlue = detail::color(3, 3, 14);
    constexpr auto MintCream = detail::color(30, 31, 31);
    constexpr auto MistyRose = detail::color(31, 28, 28);
    constexpr auto Moccasin = detail::color(31, 28, 22);
    constexpr auto NavajoWhite = detail::color(31, 27, 21);
    constexpr auto Navy = detail::color(0, 0, 16);
    constexpr auto OldLace = detail::color(31, 30, 28);
    constexpr auto Olive = detail::color(16, 16, 0);
    constexpr auto OliveDrab = detail::color(13, 17, 4);
    constexpr auto Orange = detail::color(31, 20, 0);
    constexpr auto OrangeRed = detail::color(31, 8, 0);
    constexpr auto Orchid = detail::color(27, 14, 26);
    constexpr auto PaleGoldenRod = detail::color(29, 29, 21);
    constexpr auto PaleGreen = detail::color(19, 31, 19);
    constexpr auto PaleTurquoise = detail::color(21, 29, 29);
    constexpr auto PaleVioletRed = detail::color(27, 14, 18);
    constexpr auto PapayaWhip = detail::color(31, 29, 26);
    constexpr auto PeachPuff = detail::color(31, 27, 23);
    constexpr auto Peru = detail::color(25, 16, 7);
    constexpr auto Pink = detail::color(31, 24, 25);
    constexpr auto Plum = detail::color(27, 20, 27);
    constexpr auto PowderBlue = detail::color(22, 28, 28);
    constexpr auto Purple = detail::color(16, 0, 16);
    constexpr auto RebeccaPurple = detail::color(12, 6, 19);
    constexpr auto Red = detail::color(31, 0, 0);
    constexpr auto RosyBrown = detail::color(23, 17, 17);
    constexpr auto RoyalBlue = detail::color(8, 13, 28);
    constexpr auto SaddleBrown = detail::color(17, 8, 2);
    constexpr auto Salmon = detail::color(31, 16, 14);
    constexpr auto SandyBrown = detail::color(30, 20, 12);
    constexpr auto SeaGreen = detail::color(5, 17, 10);
    constexpr auto SeaShell = detail::color(31, 30, 29);
    constexpr auto Sienna = detail::color(20, 10, 5);
    constexpr auto Silver = detail::color(24, 24, 24);
    constexpr auto SkyBlue = detail::color(16, 25, 29);
    constexpr auto SlateBlue = detail::color(13, 11, 25);
    constexpr auto SlateGray = detail::color(14, 16, 18);
    constexpr auto SlateGrey = detail::color(14, 16, 18);
    constexpr auto Snow = detail::color(31, 31, 31);
    constexpr auto SpringGreen = detail::color(0, 31, 15);
    constexpr auto SteelBlue = detail::color(8, 16, 22);
    constexpr auto Tan = detail::color(26, 22, 17);
    constexpr auto Teal = detail::color(0, 16, 16);
    constexpr auto Thistle = detail::color(27, 23, 27);
    constexpr auto Tomato = detail::color(31, 12, 8);
    constexpr auto Turquoise = detail::color(8, 28, 26);
    constexpr auto Violet = detail::color(29, 16, 29);
    constexpr auto Wheat = detail::color(30, 27, 22);
    constexpr auto White = detail::color(31, 31, 31);
    constexpr auto WhiteSmoke = detail::color(30, 30, 30);
    constexpr auto Yellow = detail::color(31, 31, 0);
    constexpr auto YellowGreen = detail::color(19, 25, 6);
}
