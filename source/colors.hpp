//--------------------------------------------------------------------------------
// colors.hpp
//--------------------------------------------------------------------------------
// Contains a list of colors
//--------------------------------------------------------------------------------
#pragma once

#include <tonc_types.h>

namespace colors
{
    inline static constexpr u16 rgb15(u8 r, u8 g, u8 b)
    {
        return r | (g << 5) | (b << 10);
    }

    constexpr auto AliceBlue = rgb15(30, 31, 31);
    constexpr auto AntiqueWhite = rgb15(31, 29, 26);
    constexpr auto Aqua = rgb15(0, 31, 31);
    constexpr auto Aquamarine = rgb15(15, 31, 26);
    constexpr auto Azure = rgb15(30, 31, 31);
    constexpr auto Beige = rgb15(30, 30, 27);
    constexpr auto Bisque = rgb15(31, 28, 24);
    constexpr auto Black = rgb15(0, 0, 0);
    constexpr auto BlanchedAlmond = rgb15(31, 29, 25);
    constexpr auto Blue = rgb15(0, 0, 31);
    constexpr auto BlueViolet = rgb15(17, 5, 28);
    constexpr auto Brown = rgb15(20, 5, 5);
    constexpr auto BurlyWood = rgb15(27, 23, 16);
    constexpr auto CadetBlue = rgb15(11, 19, 20);
    constexpr auto Chartreuse = rgb15(15, 31, 0);
    constexpr auto Chocolate = rgb15(26, 13, 3);
    constexpr auto Coral = rgb15(31, 15, 10);
    constexpr auto CornflowerBlue = rgb15(12, 18, 29);
    constexpr auto Cornsilk = rgb15(31, 31, 27);
    constexpr auto Crimson = rgb15(27, 2, 7);
    constexpr auto Cyan = rgb15(0, 31, 31);
    constexpr auto DarkBlue = rgb15(0, 0, 17);
    constexpr auto DarkCyan = rgb15(0, 17, 17);
    constexpr auto DarkGoldenRod = rgb15(23, 16, 1);
    constexpr auto DarkGray = rgb15(21, 21, 21);
    constexpr auto DarkGrey = rgb15(21, 21, 21);
    constexpr auto DarkGreen = rgb15(0, 12, 0);
    constexpr auto DarkKhaki = rgb15(23, 22, 13);
    constexpr auto DarkMagenta = rgb15(17, 0, 17);
    constexpr auto DarkOliveGreen = rgb15(10, 13, 5);
    constexpr auto DarkOrange = rgb15(31, 17, 0);
    constexpr auto DarkOrchid = rgb15(19, 6, 25);
    constexpr auto DarkRed = rgb15(17, 0, 0);
    constexpr auto DarkSalmon = rgb15(29, 18, 15);
    constexpr auto DarkSeaGreen = rgb15(17, 23, 17);
    constexpr auto DarkSlateBlue = rgb15(9, 7, 17);
    constexpr auto DarkSlateGray = rgb15(5, 9, 9);
    constexpr auto DarkSlateGrey = rgb15(5, 9, 9);
    constexpr auto DarkTurquoise = rgb15(0, 25, 26);
    constexpr auto DarkViolet = rgb15(18, 0, 26);
    constexpr auto DeepPink = rgb15(31, 2, 18);
    constexpr auto DeepSkyBlue = rgb15(0, 23, 31);
    constexpr auto DimGray = rgb15(13, 13, 13);
    constexpr auto DimGrey = rgb15(13, 13, 13);
    constexpr auto DodgerBlue = rgb15(3, 18, 31);
    constexpr auto FireBrick = rgb15(22, 4, 4);
    constexpr auto FloralWhite = rgb15(31, 31, 30);
    constexpr auto ForestGreen = rgb15(4, 17, 4);
    constexpr auto Fuchsia = rgb15(31, 0, 31);
    constexpr auto Gainsboro = rgb15(27, 27, 27);
    constexpr auto GhostWhite = rgb15(31, 31, 31);
    constexpr auto Gold = rgb15(31, 26, 0);
    constexpr auto GoldenRod = rgb15(27, 20, 4);
    constexpr auto Gray = rgb15(16, 16, 16);
    constexpr auto Grey = rgb15(16, 16, 16);
    constexpr auto Green = rgb15(0, 16, 0);
    constexpr auto GreenYellow = rgb15(21, 31, 5);
    constexpr auto HoneyDew = rgb15(30, 31, 30);
    constexpr auto HotPink = rgb15(31, 13, 22);
    constexpr auto IndianRed = rgb15(25, 11, 11);
    constexpr auto Indigo = rgb15(9, 0, 16);
    constexpr auto Ivory = rgb15(31, 31, 30);
    constexpr auto Khaki = rgb15(30, 28, 17);
    constexpr auto Lavender = rgb15(28, 28, 31);
    constexpr auto LavenderBlush = rgb15(31, 30, 30);
    constexpr auto LawnGreen = rgb15(15, 31, 0);
    constexpr auto LemonChiffon = rgb15(31, 31, 25);
    constexpr auto LightBlue = rgb15(21, 27, 28);
    constexpr auto LightCoral = rgb15(30, 16, 16);
    constexpr auto LightCyan = rgb15(28, 31, 31);
    constexpr auto LightGoldenRodYellow = rgb15(31, 31, 26);
    constexpr auto LightGray = rgb15(26, 26, 26);
    constexpr auto LightGrey = rgb15(26, 26, 26);
    constexpr auto LightGreen = rgb15(18, 29, 18);
    constexpr auto LightPink = rgb15(31, 22, 24);
    constexpr auto LightSalmon = rgb15(31, 20, 15);
    constexpr auto LightSeaGreen = rgb15(4, 22, 21);
    constexpr auto LightSkyBlue = rgb15(16, 25, 31);
    constexpr auto LightSlateGray = rgb15(14, 17, 19);
    constexpr auto LightSlateGrey = rgb15(14, 17, 19);
    constexpr auto LightSteelBlue = rgb15(22, 24, 27);
    constexpr auto LightYellow = rgb15(31, 31, 28);
    constexpr auto Lime = rgb15(0, 31, 0);
    constexpr auto LimeGreen = rgb15(6, 25, 6);
    constexpr auto Linen = rgb15(31, 30, 28);
    constexpr auto Magenta = rgb15(31, 0, 31);
    constexpr auto Maroon = rgb15(16, 0, 0);
    constexpr auto MediumAquaMarine = rgb15(12, 25, 21);
    constexpr auto MediumBlue = rgb15(0, 0, 25);
    constexpr auto MediumOrchid = rgb15(23, 10, 26);
    constexpr auto MediumPurple = rgb15(18, 14, 27);
    constexpr auto MediumSeaGreen = rgb15(7, 22, 14);
    constexpr auto MediumSlateBlue = rgb15(15, 13, 29);
    constexpr auto MediumSpringGreen = rgb15(0, 31, 19);
    constexpr auto MediumTurquoise = rgb15(9, 26, 25);
    constexpr auto MediumVioletRed = rgb15(24, 2, 16);
    constexpr auto MidnightBlue = rgb15(3, 3, 14);
    constexpr auto MintCream = rgb15(30, 31, 31);
    constexpr auto MistyRose = rgb15(31, 28, 28);
    constexpr auto Moccasin = rgb15(31, 28, 22);
    constexpr auto NavajoWhite = rgb15(31, 27, 21);
    constexpr auto Navy = rgb15(0, 0, 16);
    constexpr auto OldLace = rgb15(31, 30, 28);
    constexpr auto Olive = rgb15(16, 16, 0);
    constexpr auto OliveDrab = rgb15(13, 17, 4);
    constexpr auto Orange = rgb15(31, 20, 0);
    constexpr auto OrangeRed = rgb15(31, 8, 0);
    constexpr auto Orchid = rgb15(27, 14, 26);
    constexpr auto PaleGoldenRod = rgb15(29, 29, 21);
    constexpr auto PaleGreen = rgb15(19, 31, 19);
    constexpr auto PaleTurquoise = rgb15(21, 29, 29);
    constexpr auto PaleVioletRed = rgb15(27, 14, 18);
    constexpr auto PapayaWhip = rgb15(31, 29, 26);
    constexpr auto PeachPuff = rgb15(31, 27, 23);
    constexpr auto Peru = rgb15(25, 16, 7);
    constexpr auto Pink = rgb15(31, 24, 25);
    constexpr auto Plum = rgb15(27, 20, 27);
    constexpr auto PowderBlue = rgb15(22, 28, 28);
    constexpr auto Purple = rgb15(16, 0, 16);
    constexpr auto RebeccaPurple = rgb15(12, 6, 19);
    constexpr auto Red = rgb15(31, 0, 0);
    constexpr auto RosyBrown = rgb15(23, 17, 17);
    constexpr auto RoyalBlue = rgb15(8, 13, 28);
    constexpr auto SaddleBrown = rgb15(17, 8, 2);
    constexpr auto Salmon = rgb15(31, 16, 14);
    constexpr auto SandyBrown = rgb15(30, 20, 12);
    constexpr auto SeaGreen = rgb15(5, 17, 10);
    constexpr auto SeaShell = rgb15(31, 30, 29);
    constexpr auto Sienna = rgb15(20, 10, 5);
    constexpr auto Silver = rgb15(24, 24, 24);
    constexpr auto SkyBlue = rgb15(16, 25, 29);
    constexpr auto SlateBlue = rgb15(13, 11, 25);
    constexpr auto SlateGray = rgb15(14, 16, 18);
    constexpr auto SlateGrey = rgb15(14, 16, 18);
    constexpr auto Snow = rgb15(31, 31, 31);
    constexpr auto SpringGreen = rgb15(0, 31, 15);
    constexpr auto SteelBlue = rgb15(8, 16, 22);
    constexpr auto Tan = rgb15(26, 22, 17);
    constexpr auto Teal = rgb15(0, 16, 16);
    constexpr auto Thistle = rgb15(27, 23, 27);
    constexpr auto Tomato = rgb15(31, 12, 8);
    constexpr auto Turquoise = rgb15(8, 28, 26);
    constexpr auto Violet = rgb15(29, 16, 29);
    constexpr auto Wheat = rgb15(30, 27, 22);
    constexpr auto White = rgb15(31, 31, 31);
    constexpr auto WhiteSmoke = rgb15(30, 30, 30);
    constexpr auto Yellow = rgb15(31, 31, 0);
    constexpr auto YellowGreen = rgb15(19, 25, 6);
}
