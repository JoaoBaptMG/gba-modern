//--------------------------------------------------------------------------------
// StaticFunction.hpp
//--------------------------------------------------------------------------------
// Provides space to store statically a function object with a defined number
// of bytes, saving on allocations
//--------------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstddef>
#include <new>
#include <functional>
#include <type_traits>
#include "gba-assert.hpp"

// Utility function
namespace detail
{
    template <typename T>
    inline static T* as(void* ptr) { return std::launder(reinterpret_cast<T*>(ptr)); }

    template <typename T>
    inline static const T* as(const void* ptr) { return std::launder(reinterpret_cast<const T*>(ptr)); }
}

template <std::size_t Size, typename Sig>
class StaticFunction;

// A function holder of Size bytes and signature R(Args...)
// based on the following answer: https://stackoverflow.com/a/38478032/
template <std::size_t Size, typename R, typename... Args>
class StaticFunction<Size, R(Args...)>
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

    template <typename Functor>
    static void genericReplacer(std::byte* newObj, const std::byte* oldObj)
    {
        if (oldObj) new (newObj) Functor(*detail::as<Functor>(oldObj));
        else detail::as<Functor>(newObj)->~Functor();
    }

    static R fptrInvoker(std::byte* f, Args... args)
    {
        auto fptr = reinterpret_cast<R(**)(Args...)>(f);
        return (*fptr)(std::forward<Args>(args)...);
    }

    static void fptrReplacer(std::byte* newObj, const std::byte* oldObj)
    {
        *reinterpret_cast<R(**)(Args...)>(newObj) =
            *reinterpret_cast<R(* const*)(Args...)>(oldObj);
    }

    // Now define the pointers
    Invoker invoker;
    Replacer replacer;

    // And finally the storage
    alignas(void*) std::byte storage[Size];

public:
    // A trivial default constructor
    StaticFunction() = default;

    // A constructor for function pointers
    StaticFunction(R (*f)(Args...)) : invoker(fptrInvoker), replacer(fptrReplacer)
    {
        // Copy the function pointer
        replacer(storage, reinterpret_cast<const std::byte*>(&f));
    }

    // A templated constructor for any callable object
    template <typename Functor>
    StaticFunction(const Functor& f) : invoker(genericInvoker<Functor>), replacer(genericReplacer<Functor>)
    {
        static_assert(std::is_invocable_r_v<R, Functor, Args...>,
            "Functor must be callable with the appropriate signature!");
        static_assert(sizeof(Functor) <= Size,
            "The required function object is too big to be stored!");

        // Copy the functor
        replacer(storage, reinterpret_cast<const std::byte*>(&f));
    }

    // Copy constructor
    StaticFunction(const StaticFunction& other) : invoker(other.invoker), replacer(other.replacer)
    {
        // Replace this one storage with the other
        if (replacer) replacer(storage, other.storage);
    }

    // Copy assignment operator
    StaticFunction& operator=(const StaticFunction& other)
    {
        // Destroy the object here first
        if (replacer) replacer(storage, nullptr);
        invoker = other.invoker;
        replacer = other.replacer;
        replacer(storage, other.storage);
        return *this;
    }

    // Call operator
    R operator()(Args... args)
    {
        ASSERT(invoker);
        return invoker(storage, std::forward<Args>(args)...);
    }

    // Destructor
    ~StaticFunction()
    {
        replacer(storage, nullptr);
    }
};
