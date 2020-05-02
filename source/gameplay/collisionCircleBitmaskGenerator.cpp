//--------------------------------------------------------------------------------
// collisionCircleBitmapGenerator.cpp
//--------------------------------------------------------------------------------
// Provide collision detection routines and utilities
//--------------------------------------------------------------------------------
#include "math/stdfixed.hpp"
#include "math/vec2.hpp"

#include <array>
#include <cstdint>
#include <algorithm>
#include <type_traits>

#include "data/BitmaskData.hpp"

#include "text/mGBADebugging.hpp"

// Necessary, otherwise gcc will not respect the 4-byte alignment necessary
template <std::size_t N>
static constexpr auto align32(std::array<u16, N> data)
{
    std::array<u32, (N+1)/2> aligned{};
    for (std::size_t i = 0; i < N; i++)
        aligned[i/2] |= data[i] << (16 * (i%2));

    return aligned;
}

template <std::size_t Radius>
static constexpr auto generateCircleBitmask()
{
    constexpr auto ClusterSize = (2*Radius+15)/16;
    constexpr auto PxSize = 2*Radius;

    std::array<u16, ClusterSize*PxSize+2> data{};
    data[0] = PxSize << 7;
    data[1] = PxSize << 7;

    for (std::size_t j = 0; j < PxSize; j++)
        for (std::size_t i = 0; i < PxSize; i++)
        {
            auto ti = i + 0.5 - Radius;
            double tj = j + 0.5 - Radius;

            if (ti*ti + tj*tj <= Radius*Radius)
                data[2 + ClusterSize*j + (i/16)] |= 1 << (i%16);
        }

    return align32(data);
}

template <std::size_t... Rs>
static constexpr auto collectBitmasks()
{
    constexpr auto Ids = std::index_sequence<Rs...>();
    constexpr auto Arrays = std::make_tuple(generateCircleBitmask<Rs>()...);
    return std::make_tuple(Ids, Arrays);
}

template <typename T>
struct MaxOfSequence {};

template <std::size_t... Nums>
struct MaxOfSequence<std::index_sequence<Nums...>>
    : std::integral_constant<std::size_t, std::max({Nums...})> {};

template <typename T, std::size_t Size, std::size_t... Indices, std::size_t... Is>
static constexpr void setBitmasks(std::array<const void*, Size>& arr, const T& values, 
    std::index_sequence<Indices...>, std::index_sequence<Is...>)
{
    ((arr[Indices] = &std::get<Is>(values)), ...);
}

template <typename T>
static constexpr auto attachBitmasks(const T& values)
{
    const auto& bitmasks = std::get<1>(values);
    using Ids = std::decay_t<decltype(std::get<0>(values))>;
    std::array<const void*, MaxOfSequence<Ids>::value+1> array{};

    constexpr auto Size = Ids::size();
    setBitmasks(array, bitmasks, Ids{}, std::make_index_sequence<Size>{});

    return array;
}

constexpr static const auto dCircleBitmasks = collectBitmasks<1, 2, 8>();
constexpr extern const auto CircleBitmasks = attachBitmasks(dCircleBitmasks);

extern "C" void assertRadiusDoesNotExist(std::size_t dummy, std::size_t radius)
{
    if (mgba::isEnabled())
        mgba::log(mgba::Log::Fatal, "Circle bitmask was not generated for radius ", radius);

    for (;;) asm volatile ("" ::: "memory");
}
