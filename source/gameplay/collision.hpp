//--------------------------------------------------------------------------------
// collision.hpp
//--------------------------------------------------------------------------------
// Provides functions for collision detection of various forms
//--------------------------------------------------------------------------------
#pragma once

#include "math/stdfixed.hpp"
#include "math/sqrtfixed.hpp"
#include "math/vec2.hpp"
#include "util/type_traits.hpp"

template <typename T> T sign(T v) { return (v > 0) - (v < 0); }
template <typename T> bool abscmp(T v1, T v2) { return abs(v1) < abs(v2); }

namespace collision
{
    enum class ResponseType { YesNo, Response, Normal };

    template <ResponseType Response>
    using Ret = std::conditional_t<Response == ResponseType::YesNo, bool, vec2<s32f8>>;

    // Check for collision between two boxes
    template <ResponseType Response = ResponseType::YesNo>
    Ret<Response> box(vec2<s32f8> pos1, vec2<s32f8> size1, vec2<s32f8> pos2, vec2<s32f8> size2)
    {
        auto br1 = pos1 + size1;
        auto br2 = pos2 + size2;
        if (br1.x <= pos2.x || br1.y <= pos2.y || br2.x <= pos1.x || br2.y <= pos1.y) return Ret<Response>();

        if constexpr (Response == ResponseType::YesNo) return true;
        else
        {
            auto hdist = std::min(pos2.x - br1.x, br2.x - pos1.x, abscmp<s32f8>);
            auto vdist = std::min(pos2.y - br1.y, br2.y - pos1.y, abscmp<s32f8>);
            if constexpr (Response == ResponseType::Response)
                return abs(hdist) <= abs(vdist) ? vec2<s32f8>(hdist, 0) : vec2<s32f8>(0, vdist);
            else return abs(hdist) <= abs(vdist) ? vec2<s32f8>(hdist > 0 ? 1 : -1, 0) : vec2<s32f8>(0, vdist > 0 ? 1 : -1);
        }
    }

    // Check for collision between two circles
    template <ResponseType Response = ResponseType::YesNo>
    Ret<Response> circle(vec2<s32f8> pos1, s32f8 r1, vec2<s32f8> pos2, s32f8 r2)
    {
        // Quick bbox check
        if (pos1.x + r1 <= pos2.x - r2 || pos1.y + r1 <= pos2.y - r2 ||
            pos2.x + r2 <= pos1.x - r1 || pos2.y + r2 <= pos1.y - r1) return Ret<Response>();

        // Calculate the necessary values
        auto dp = pos1 - pos2;
        auto sr = r1 + r2;
        auto lensq = dp.lensq();
        if (lensq > sr*sr) return Ret<Response>();

        // Here we have necessarily a division and a square root. It could be done otherwise? Maybe
        if constexpr (Response == ResponseType::YesNo) return true;
        else if constexpr (Response == ResponseType::Response)
            return dp * sr / sqrt(lensq) - dp;
        else return vec2<s32f16>(dp) / sqrt(lensq);
    }

    // Check for collision between a circle and a box
    template <ResponseType Response = ResponseType::YesNo>
    Ret<Response> circleBox(vec2<s32f8> pos1, s32f8 r1, vec2<s32f8> pos2, vec2<s32f8> size2)
    {
        // Bail out if their bboxes don't even meet
        vec2 rDiag(r1, r1);
        auto res = box<Response>(pos1 - rDiag, 2*rDiag, pos2, size2);
        if (res == Ret<Response>()) return res;

        // Now, check the four edges
        auto topLeft = pos2;
        auto bottomRight = pos2 + size2;
        auto topRight = vec2(bottomRight.x, topLeft.y);
        auto bottomLeft = vec2(topLeft.x, bottomRight.y);

        vec2<s32f8> rLeft(-r1, 0), rUp(0, -r1);

        if (box<>(pos1, vec2<s32f8>(), topLeft - rDiag, rDiag))
            return circle<Response>(pos1, r1, topLeft, 0);
        if (box<>(pos1, vec2<s32f8>(), topRight + rUp, rDiag))
            return circle<Response>(pos1, r1, topRight, 0);
        if (box<>(pos1, vec2<s32f8>(), bottomLeft + rLeft, rDiag))
            return circle<Response>(pos1, r1, bottomLeft, 0);
        if (box<>(pos1, vec2<s32f8>(), bottomRight, rDiag))
            return circle<Response>(pos1, r1, bottomRight, 0);

        return res;
    }

    // Check for collision between two swept boxes, without response vector
    template <ResponseType Response = ResponseType::YesNo>
    Ret<Response> sweptBox(vec2<s32f8> pos1, vec2<s32f8> vel1, vec2<s32f8> size1, vec2<s32f8> pos2, vec2<s32f8> vel2, vec2<s32f8> size2)
    {
        return Ret<Response>();
    }
}
