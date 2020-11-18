#pragma once

#include "../math/stdfixed.hpp"
#include "../math/vec2.hpp"
#include "../math/fixedmath.hpp"
#include "../math/gcem-degrees.hpp"

namespace vec_utils
{
    inline static vec2<s32f16> normalizeLowp(vec2<s32f16> vec)
    {
        auto lensq = vec2<s32f8>(vec).lensq();
        auto reclen = reciprocal(sqrt(lensq)).with_exp<8>();
        return vec2<s32f16>(vec * reclen);
    }

    template <auto Degrees, typename T>
    inline static constexpr vec2<T> rotate_d(vec2<T> v)
    {
        using NT = decltype(std::declval<T>().template with_exp<10>());
        constexpr NT cos = gcem_d::cos(Degrees);
        constexpr NT sin = gcem_d::sin(Degrees);
        return vec2<T>(v.x * cos - v.y * sin, v.x * sin + v.y * cos);
    }
}