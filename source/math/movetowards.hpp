//--------------------------------------------------------------------------------
// movetowards.hpp
//--------------------------------------------------------------------------------
// Provides the moveTowards function, to ease some logic that requires it
//--------------------------------------------------------------------------------
#pragma once

template <typename T>
void moveTowards(T& cur, const T& target, const T& step = T(1))
{
    if (cur >= target - step && cur <= target + step)
        cur = target;
    else if (cur < target) cur += step;
    else if (cur > target) cur -= step;
}
