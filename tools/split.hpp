#pragma once

#include <string>
#include <string_view>
#include <iterator>

template <typename CharT>
class basic_string_splitter final
{
    using sstr = std::basic_string<CharT>;
    using sview = std::basic_string_view<CharT>;
    using stype = typename sview::size_type;

    sstr str;
    CharT delim;

public:
    class iterator final
    {
        const basic_string_splitter& splitter;
        stype lastpos, pos;

    public:
        using difference_type = std::intptr_t;
        using value_type = sview;
        using pointer = const sview*;
        using reference = sview;
        using iterator_category = std::forward_iterator_tag;

        iterator(const basic_string_splitter& splitter, stype lastpos, stype pos)
            : splitter(splitter), lastpos(lastpos), pos(pos) {}

        // Increment
        iterator& operator++()
        {
            if (pos != stype(-1))
            {
                lastpos = pos+1;
                pos = splitter.str.find(splitter.delim, lastpos);
            }
            else lastpos = pos = stype(-1);
            return *this;
        }

        reference operator*() const
        {
            return sview(splitter.str).substr(lastpos, pos - lastpos);
        }

        pointer operator->() const { return &operator*(); }

        // Equality
        bool operator==(const iterator& o) const
        {
            return splitter == o.splitter && lastpos == o.lastpos && pos == o.pos;
        }
        bool operator!=(const iterator& o) const { return !(*this == o); }
        
        // Post-decrement
        iterator operator++(int)
        {
            iterator it(*this);
            operator++();
            return it;
        }
    };

    basic_string_splitter(sstr str, CharT delim) : str(str), delim(delim) {}
    auto begin() const { return iterator(*this, stype(0), str.find(delim)); }
    auto end() const { return iterator(*this, stype(-1), stype(-1)); }
    auto cbegin() const { return begin(); }
    auto cend() const { return end(); }

    bool operator==(const basic_string_splitter& o) const
    {
        return str == o.str && delim == o.delim;
    }
};

using string_splitter = basic_string_splitter<char>;

template <typename CharT>
auto split(std::basic_string<CharT> str, CharT delim = ' ')
{
    return basic_string_splitter<CharT>(std::move(str), delim);
}
