//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <type_traits>

namespace util
{
    template <typename T>
    class grid final
    {
        class view final
        {
            class iterator final
            {
                view* ref;
                std::size_t i, j;

                iterator(view *ref, std::size_t i, std::size_t j) : ref(ref), i(i), j(j) {}

            public:
                using difference_type = std::intmax_t;
                using value_type = T;
                using pointer = T*;
                using reference = T&;
                using iterator_category = std::random_access_iterator_tag;

                iterator() : ref(nullptr), i(0), j(0) {}

                iterator& operator++()
                {
                    i++;
                    if (i >= ref->_width) { i = 0; j++; }
                    return *this;
                }

                iterator operator++(int)
                {
                    iterator it(*this);
                    ++(*this);
                    return it;
                }

                iterator& operator--()
                {
                    if (i == 0)
                    {
                        if (j > 0) { j--; i = ref->_width-1; }
                    }
                    else i--;
                    return *this;
                }

                iterator operator--(int)
                {
                    iterator it(*this);
                    --(*this);
                    return it;
                }

                bool operator==(const iterator &other) const
                {
                    return ref == other.ref && i == other.i && j == other.j;
                }
                bool operator!=(const iterator &other) const { return !(*this == other); }

                iterator operator+(std::intmax_t val) const
                {
                    std::size_t offset = i + val;
                    return iterator(ref, offset % ref->_width, j + offset / ref->_width);
                }

                iterator& operator+=(std::intmax_t val)
                {
                    std::size_t offset = i + val;
                    i = offset % ref->_width;
                    j += offset / ref->_width;
                    return *this;
                }

                iterator operator-(std::intmax_t val) const { return *this + (-val); }
                iterator& operator-=(std::intmax_t val) { return *this += (-val); }

                std::intmax_t operator-(const iterator& other) const
                {
                    if (ref != other.ref) return 0;

                    std::size_t ofs1 = j*ref->_width + i;
                    std::size_t ofs2 = other.j*ref->_width + other.i;

                    return ofs1 - ofs2;
                }

                T& operator*() { return (*ref)(i,j); }
                const T& operator*() const { return (*ref)(i,j); }

                T* operator->() { return &(*ref)(i,j); }
                const T* operator->() const { return &(*ref)(i,j); }

                T& operator[](std::intmax_t ind) { return *(*this + ind); }
                const T& operator[](std::intmax_t ind) const { return *(*this + ind); }

                bool operator<(const iterator& other) const { return other - *this > 0; }
                bool operator>(const iterator& other) const { return other < *this; }
                bool operator>=(const iterator& other) const { return !(*this < other); }
                bool operator<=(const iterator& other) const { return !(*this > other); }

                friend class grid<T>::view;
            };

            using const_iterator = const iterator;

            grid* ref;
            std::size_t x, y, _width, _height;

            view (grid* ref, std::size_t x, std::size_t y, std::size_t w, std::size_t h)
            : ref(ref), x(x), y(y), _width(w), _height(h) {}

        public:
            T& operator()(std::size_t i, std::size_t j) { return (*ref)(x+i, y+j); }
            const T& operator()(std::size_t i, std::size_t j) const { return (*ref)(x+i, y+j); }

            T& at(std::size_t i, std::size_t j)
            {
                if (i >= _width || j >= _height)
                    throw std::out_of_range("Attempt to access element outside of bounds of the grid!");
                return operator()(i, j);
            }

            const T& at(std::size_t i, std::size_t j) const
            {
                if (i >= _width || j >= _height)
                    throw std::out_of_range("Attempt to access element outside of bounds of the grid!");
                return operator()(i, j);
            }

            const_iterator cbegin() const { return iterator(const_cast<view*>(this), 0, 0); }
            const_iterator cend() const { return iterator(const_cast<view*>(this), 0, _height); }

            const_iterator begin() const { return cbegin(); }
            const_iterator end() const { return cend(); };

            iterator begin() { return iterator(this, 0, 0); }
            iterator end() { return iterator(this, 0, _height); }

            bool operator==(const view& other) const
            {
                return ref == other.ref
                    && x == other.x
                    && y == other.y
                    && _width == other._width
                    && _height == other._height;
            }

            bool operator!=(const view& other) const { return !(*this == other); }

            friend class grid<T>;
        };

        std::size_t _width, _height;
        T* elements;

    public:
        grid() noexcept : _width(0), _height(0), elements(nullptr) {}
        grid(std::size_t w, std::size_t h) : _width(w), _height(h), elements(new T[w*h])
        {
            std::fill(begin(), end(), T());
        }
        grid(std::size_t w, std::size_t h, T* contents) : grid(w, h)
        {
            std::copy(contents, contents+(w*h), elements);
        }

        template <typename I>
        grid(std::size_t w, std::size_t h, I begin, I end) : grid(w, h)
        {
            std::size_t i = 0;
            for (auto it = begin; it != end; ++it)
            {
                if (i >= w*h) break;
                elements[i++] = *it;
            }
        }

        grid(const view &v) : grid(v._width, v._height, v.cbegin(), v.cend()) {}

        grid(const grid& other) : grid(other._width, other._height, other.elements) {}
        grid(grid&& other) noexcept : grid() { swap(*this, other); }

        grid& operator=(grid other) noexcept
        {
            swap(*this, other);
            return *this;
        }

        friend void swap(grid& g1, grid& g2) noexcept
        {
            using std::swap;
            swap(g1._width, g2._width);
            swap(g1._height, g2._height);
            swap(g1.elements, g2.elements);
        }

        ~grid() { delete[] elements; }

        T& operator()(std::size_t i, std::size_t j) { return elements[j*_width+i]; }
        const T& operator()(std::size_t i, std::size_t j) const { return elements[j*_width+i]; }

        T& at(std::size_t i, std::size_t j)
        {
            if (i >= _width || j >= _height)
                throw std::out_of_range("Attempt to access element outside of bounds of the grid!");
            return operator()(i, j);
        }

        const T& at(std::size_t i, std::size_t j) const
        {
            if (i >= _width || j >= _height)
                throw std::out_of_range("Attempt to access element outside of bounds of the grid!");
            return operator()(i, j);
        }

        void resize(std::size_t w, std::size_t h)
        {
            if (_width == w && _height == h) return;

            grid<T> g(w, h);
            auto minw = std::min(w, _width);
            auto minh = std::min(h, _height);
            for (std::size_t j = 0; j < minh; j++)
                std::copy_n(elements + _height*j, minw, g.elements + h*j);

            swap(*this, g);
        }

        T* data() { return elements; }
        const T* data() const { return elements; }

        const T* cbegin() const { return elements; }
        const T* cend() const { return elements+(_width*_height); }

        const T* begin() const { return cbegin(); }
        const T* end() const { return cend(); }

        T* begin() { return elements; }
        T* end() { return elements+(_width*_height); }

        const auto make_view(std::size_t x, std::size_t y, std::size_t _width, std::size_t _height) const
        {
            if (x >= this->_width || y >= this->_height ||
                x + _width > this->_width || y + _height > this->_height)
                throw std::out_of_range("Attempt to make a view that spans outside of the bounds of the grid!");

            return view(const_cast<grid<T>*>(this), x, y, _width, _height);
        }

        auto make_view(std::size_t x, std::size_t y, std::size_t _width, std::size_t _height)
        {
            if (x >= this->_width || y >= this->_height ||
                x + _width > this->_width || y + _height > this->_height)
                throw std::out_of_range("Attempt to make a view that spans outside of the bounds of the grid!");

            return view(this, x, y, _width, _height);
        }

        std::size_t width() const { return _width; }
        std::size_t height() const { return _height; }
        
        bool empty() const { return _width == 0 || _height == 0; }
    };

    template <typename T, bool Const>
    typename grid<T>::view::iterator& operator+(std::intmax_t val,
        typename grid<T>::view::iterator &it)
    {
        return it + val;
    }
}
