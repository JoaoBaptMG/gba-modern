//--------------------------------------------------------------------------------
// mGBADebugging.hpp
//--------------------------------------------------------------------------------
// Add support for debugging in the mGBA log file
//--------------------------------------------------------------------------------
#pragma once

#include "StringBuilder.hpp"

#define REG_DEBUG_ENABLE (vu16*)0x4FFF780
#define REG_DEBUG_FLAGS (vu16*)0x4FFF700
#define REG_DEBUG_STRING (char*)0x4FFF600

namespace mgba
{
    enum class Log : u16 { Fatal = 0, Error, Warn, Info, Debug };

    inline static void enable()
    {
        *REG_DEBUG_ENABLE = 0xC0DE;
    }

    inline static bool isEnabled()
    {
        return *REG_DEBUG_ENABLE == 0x1DEA;
    }

    inline static void disable()
    {
        *REG_DEBUG_ENABLE = 0;
    }

    template <typename... Ts>
    inline static void log(Log level, Ts&&... vs)
    {
        InplaceStringBuilder<255> sb(REG_DEBUG_STRING);
        sb.append(std::forward<Ts>(vs)...);
        sb.getString(); // Important, to set the last equal to zero
        *REG_DEBUG_FLAGS = static_cast<u16>(level) | 0x100;
    }
}
