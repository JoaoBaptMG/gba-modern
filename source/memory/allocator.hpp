//--------------------------------------------------------------------------------
// allocator.hpp
//--------------------------------------------------------------------------------
// Provides a very rudimentary allocator for memory in EWRAM
//--------------------------------------------------------------------------------
#pragma once

#include <cstddef>
#include <memory>
#include <tonc.h>

namespace ewram
{
    void init() EWRAM_CODE;

    // Allocates a chunk of size sz in the EWRAM
    void* allocate(std::size_t sz) IWRAM_CODE;

    // Frees a chunk of memory that was previously allocated
    void free(void* ptr, std::size_t sz) IWRAM_CODE;

    // Define a specific unique_ptr for it
    template <typename T>
    struct Deleter
    {
        void operator()(T* ptr) { if (ptr) free(ptr, sizeof(T)); }
    };

    template <typename T>
    struct ArrayDeleter
    {
        std::size_t n;
        void operator()(T* ptr) { if (ptr) free(ptr, n*sizeof(T)); }
    };

    template <typename T>
    struct unique_ptr_decider
    {
        using type = std::unique_ptr<T, Deleter<T>>;
        using is_single_object = type;
    };

    template <typename T>
    struct unique_ptr_decider<T[]>
    {
        using type = std::unique_ptr<T[], ArrayDeleter<T>>;
        using is_unknown_bound = type;
    };

    template <typename T>
    using unique_ptr = typename unique_ptr_decider<T>::type;

    // And a make_unique
    template<typename T, typename... Args>
    typename unique_ptr_decider<T>::is_single_object
    make_unique(Args&&... args)
    {
        return unique_ptr<T>(new (allocate(sizeof(T))) T(std::forward<Args>(args)...));
    }

    template<typename T>
    typename unique_ptr_decider<T>::is_unknown_bound
    make_unique(std::size_t n)
    {
        using U = std::remove_extent_t<T>;
        return unique_ptr<T>(new (allocate(n*sizeof(U))) U[n], {n});
    }
}

struct in_ewram_tag {};

// Overloads for operators new and delete
inline void* operator new(std::size_t sz, in_ewram_tag) noexcept { return ewram::allocate(sz); }
inline void* operator new[](std::size_t sz, in_ewram_tag) noexcept { return ewram::allocate(sz); }
inline void operator delete(void* ptr, std::size_t sz, in_ewram_tag) noexcept { ewram::free(ptr, sz); }
inline void operator delete[](void* ptr, std::size_t sz, in_ewram_tag) noexcept { ewram::free(ptr, sz); }
