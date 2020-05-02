//--------------------------------------------------------------------------------
// DataPng.hpp
//--------------------------------------------------------------------------------
// Base class for the "naming convention" used by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>

#include "BitmaskData.hpp"

template <std::size_t DataCount, std::size_t ColorCount>
struct DataPng final
{
    struct __png
    {
        std::uint8_t tiles[DataCount];
        std::uint16_t palette[ColorCount];
    } png;
};

template <std::size_t DataCount>
struct DataPng<DataCount, 0> final
{
    struct __png
    {
        std::uint8_t tiles[DataCount];
    } png;
};

template <std::size_t DataCount, std::size_t ColorCount>
struct BitmaskPng final
{
    struct __png
    {
        std::uint8_t tiles[DataCount];
        std::uint16_t palette[ColorCount];
        BitmaskData bitmask;
    } png;
};

template <std::size_t DataCount>
struct BitmaskPng<DataCount, 0> final
{
    struct __png
    {
        std::uint8_t tiles[DataCount];
        BitmaskData bitmask;
    } png;
};

