#pragma once

#include "math/stdfixed.hpp"
#include "math/vec2.hpp"
#include "math/fixedmath.hpp"
#include "math/gcem-degrees.hpp"

namespace vec_utils
{
    inline static vec2<s32f16> normalizeLowp(vec2<s32f16> vec)
    {
        auto lensq = vec2<s32f8>(vec).lensq();
        auto reclen = reciprocal(sqrt(lensq)).with_exp<8>();
        return vec2<s32f16>(vec * reclen);
    }

    inline static constexpr vec2<fixed<s32, 10>> getRotator_d(double d)
    {
        return vec2<fixed<s32, 10>>(gcem_d::cos(d), gcem_d::sin(d));
    }
#define ROTATE_VEC2(v, d) (vec2<decltype((v).x)>(vec_utils::getRotator_d(d).rotate(v)))
}