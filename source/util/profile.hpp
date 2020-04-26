//--------------------------------------------------------------------------------
// profile.hpp
//--------------------------------------------------------------------------------
// Profiling functions
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>

namespace profile
{
    enum class Precision { Cycles, CyclesOver64, CyclesOver256, CyclesOver1024 };

    void begin16(Precision precision = Precision::Cycles);
    [[nodiscard]] std::uint16_t end16();

    void begin32(Precision precision = Precision::Cycles);
    [[nodiscard]] std::uint16_t end32();
}
