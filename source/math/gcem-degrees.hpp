//--------------------------------------------------------------------------------
// gcem-degrees.hpp
//--------------------------------------------------------------------------------
// Provides most of the trigonometric functions of GCE-Math in degrees
//--------------------------------------------------------------------------------
#pragma once

#include <gcem.hpp>

namespace gcem_d
{
    template <typename T>
    constexpr auto rad2deg = gcem::return_t<T>(57.295779513082320876798154814105);
    
    template <typename T>
    constexpr auto sin(T x) { return gcem::sin(x / rad2deg<T>); }

    template <typename T>
    constexpr auto cos(T x) { return gcem::cos(x / rad2deg<T>); }

    template <typename T>
    constexpr auto tan(T x) { return gcem::tan(x / rad2deg<T>); }

    template <typename T>
    constexpr auto asin(T x) { return gcem::asin(x) * rad2deg<T>; }

    template <typename T>
    constexpr auto acos(T x) { return gcem::acos(x) * rad2deg<T>; }

    template <typename T>
    constexpr auto atan(T x) { return gcem::atan(x) * rad2deg<T>; }

    template <typename T>
    constexpr auto atan2(T y, T x) { return gcem::atan(y, x) * rad2deg<T>; }
}
