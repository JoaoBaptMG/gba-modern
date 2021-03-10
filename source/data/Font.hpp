//--------------------------------------------------------------------------------
// Font.hpp
//--------------------------------------------------------------------------------
// Provides the Font structure to be populated by the exporter tool
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <array>
#include "util/type_traits.hpp"
#include "util/integer-sequence-utils.hpp"
#include "GlyphData.hpp"

namespace detail::font
{
    template <std::size_t _First, std::size_t _Last>
    struct GlyphInterval
    {
        constexpr static const auto First = _First;
        constexpr static const auto Last = _Last;
        constexpr static const auto Size = _Last - _First + 1;
    };

    template <typename T>
    struct IsGlyphInterval : std::false_type {};

    template <std::size_t _First, std::size_t _Last>
    struct IsGlyphInterval<GlyphInterval<_First, _Last>> : std::true_type {};

    template <std::size_t _VerticalStride, typename... GlyphIntervals>
    struct Font
    {
        static_assert((IsGlyphInterval<GlyphIntervals>::value && ...),
            "All types in the parameter pack must be GlyphIntervals!");
        
        constexpr static const auto VerticalStride = _VerticalStride;
        constexpr static const auto NumGlyphs = (GlyphIntervals::Size + ...);
        GlyphData glyphs[NumGlyphs];

    private:
        using SizeSeq = std::index_sequence<GlyphIntervals::Size...>;
        using OffsetSeq = prefix_sum_t<SizeSeq>;

        template <std::size_t... Offsets>
        constexpr const GlyphData& glyphForHelper(std::size_t ch, std::index_sequence<Offsets...>) const
        {
            using GI = std::array<std::size_t, 3>;
            for (auto gi : { GI{GlyphIntervals::First, GlyphIntervals::Last, Offsets}... })
            {
                auto [glyphFirst, glyphLast, offset] = gi;
                if (ch >= glyphFirst && ch <= glyphLast)
                    return glyphs[offset + ch - glyphFirst];
            }

            return glyphs[0];
        }

    public:
        constexpr const GlyphData& glyphFor(std::size_t ch) const
        {
            return glyphForHelper(ch, OffsetSeq{});
        }
    };

    template <typename T>
    struct IsFont : std::false_type {};

    template <std::size_t _VerticalStride, typename... GlyphIntervals>
    struct IsFont<Font<_VerticalStride, GlyphIntervals...>> : std::true_type {};
}

using detail::font::Font;

template <typename T>
constexpr static auto IsFont = detail::font::IsFont<T>::value;

template <std::size_t VerticalStride, typename... GlyphIntervals>
struct FontHandle final
{
    Font<VerticalStride, GlyphIntervals...> ttf;
};
