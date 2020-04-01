//--------------------------------------------------------------------------------
// TrivialStaticFunction.hpp
//--------------------------------------------------------------------------------
// Provides space to store statically a function object with a defined number
// of bytes, saving on allocations. This differs from StaticFunction in that
// every object must be trivially copiable and trivially destructible
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstddef>
#include <new>
#include <type_traits>
#include <tonc.h>

// Utility function
namespace detail
{
    template <typename T>
    inline static T* as(void* ptr) { return std::launder(reinterpret_cast<T*>(ptr)); }

    template <typename T>
    inline static const T* as(const void* ptr) { return std::launder(reinterpret_cast<const T*>(ptr)); }
}

template <std::size_t Size, typename Sig>
class TrivialStaticFunction;

// A function holder of Size bytes and signature R(Args...)
// based on the following answer: https://stackoverflow.com/a/38478032/
template <std::size_t Size, typename R, typename... Args>
class TrivialStaticFunction<Size, R(Args...)>
{
    // Define the important function pointers here
    using Invoker = R(*)(std::byte*, Args...);
    using Replacer = void(*)(std::byte*, const std::byte*);

    template <typename Functor>
    static R genericInvoker(std::byte* f, Args... args)
    {
        static_assert(std::is_invocable_r_v<R, Functor, Args...>,
            "Functor must be callable with the appropriate signature!");
        return std::invoke(*detail::as<Functor>(f), std::forward<Args>(args)...);
    }

    static R fptrInvoker(std::byte* f, Args... args)
    {
        auto fptr = reinterpret_cast<R(**)(Args...)>(f);
        return (*fptr)(std::forward<Args>(args)...);
    }

    // Now define the pointer
    Invoker invoker;

    // And finally the storage
    alignas(void*) std::byte storage[Size];

public:
    // A trivial default constructor
    TrivialStaticFunction() = default;

    // A constructor for function pointers
    TrivialStaticFunction(R (*f)(Args...)) : invoker(fptrInvoker)
    {
        // Copy the function pointer
        *reinterpret_cast<R(**)(Args...)>(storage) = f;
    }

    // A templated constructor for any callable object
    template <typename Functor>
    TrivialStaticFunction(const Functor& f) : invoker(genericInvoker<Functor>)
    {
        static_assert(std::is_invocable_r_v<R, Functor, Args...>,
            "Functor must be callable with the appropriate signature!");
        static_assert(sizeof(Functor) <= Size,
            "The required function object is too big to be stored!");
        static_assert(std::is_trivially_copyable_v<Functor>,
            "The required function object must be trivially copyable!");
        static_assert(std::is_trivially_destructible_v<Functor>,
            "The required function object must be trivially destructible!");

        // Copy the functor
        if constexpr (sizeof(Functor) % 4 == 0 && alignof(Functor) % 4 == 0)
            memcpy32(storage, &f, sizeof(Functor)/sizeof(u32));
        else if constexpr (sizeof(Functor) % 2 == 0 && alignof(Functor) % 2 == 0)
            memcpy16(storage, &f, sizeof(Functor)/sizeof(u16));
        else memcpy(storage, &f, sizeof(Functor));
    }

    // Copy constructor
    TrivialStaticFunction(const TrivialStaticFunction& other) : invoker(other.invoker)
    {
        // Replace this one storage with the other
        if constexpr (Size % 4 == 0)
            memcpy32(storage, other.storage, Size/sizeof(u32));
        else if constexpr (Size % 2 == 0)
            memcpy16(storage, other.storage, Size/sizeof(u16));
        else memcpy(storage, other.storage, Size);
    }

    // A copy operator for function pointers
    TrivialStaticFunction& operator=(R (*f)(Args...))
    {
        // Copy the function pointer
        invoker = fptrInvoker;
        *reinterpret_cast<R(**)(Args...)>(storage) = f;
        return *this;
    }

    // A templated copy operator for any callable object
    template <typename Functor>
    TrivialStaticFunction& operator=(const Functor& f)
    {
        static_assert(std::is_invocable_r_v<R, Functor, Args...>,
            "Functor must be callable with the appropriate signature!");
        static_assert(sizeof(Functor) <= Size,
            "The required function object is too big to be stored!");
        static_assert(std::is_trivially_copyable_v<Functor>,
            "The required function object must be trivially copyable!");
        static_assert(std::is_trivially_destructible_v<Functor>,
            "The required function object must be trivially destructible!");

        // Copy the functor
        invoker = genericInvoker<Functor>;
        if constexpr (sizeof(Functor) % 4 == 0 && alignof(Functor) % 4 == 0)
            memcpy32(storage, &f, sizeof(Functor)/sizeof(u32));
        else if constexpr (sizeof(Functor) % 2 == 0 && alignof(Functor) % 2 == 0)
            memcpy16(storage, &f, sizeof(Functor)/sizeof(u16));
        else memcpy(storage, &f, sizeof(Functor));
        return *this;
    }

    // Copy assignment operator
    TrivialStaticFunction& operator=(const TrivialStaticFunction& other)
    {
        invoker = other.invoker;
        // Replace this one storage with the other
        if constexpr (Size % 4 == 0)
            memcpy32(storage, other.storage, Size/sizeof(u32));
        else if constexpr (Size % 2 == 0)
            memcpy16(storage, other.storage, Size/sizeof(u16));
        else memcpy(storage, other.storage, Size);
        return *this;
    }

    // Call operator
    R operator()(Args... args)
    {
        ASSERT(invoker);
        return invoker(storage, std::forward<Args>(args)...);
    }

    // Destructor
    ~TrivialStaticFunction() = default;
};
