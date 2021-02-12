//--------------------------------------------------------------------------------
// type_traits.hpp
//--------------------------------------------------------------------------------
// Provides the original type_traits header, plus some traits copied from
// boost, since I do not want to include boost in a GBA project
//--------------------------------------------------------------------------------
#pragma once

#include <type_traits>

namespace detail
{
    // Definition of is_virtual_base according to boost:

    /* Quote from https://www.boost.org/doc/libs/1_68_0/boost/type_traits/is_virtual_base_of.hpp:
        Implementation based on the standard's rules of explicit type conversions.
        A pointer to an object of *derived* class type may be explicitly converted to a pointer to an *unambiguous* *base* class type.
        A pointer to an object of an *unambiguous* *non-virtual* *base* class type may be explicitly converted to a pointer of a *derived* class type.
        Therefore Derived has a virtual base Base if and only if
            (1) a Derived* can be converted to Base* (so the base class is unambiguous, which comes necessarily from virtual inheritance)
            (2) a Base* cannot be converted to Derived* (so the base class is either ambiguous or virtual)
        With both conditions true, Base must be a virtual base of Derived.
        The "is_base_of" is only needed so the compiler can (but is not required to) error out if the types are incomplete.
        This is in league with the the expected behaviour. 
        
        C-style casts have the power to ignore inheritance visibility while still act as a static_cast.
        They can also fall back to the behaviour of reinterpret_cast, which allows is_virtual_base_of to work on non-class types too.
        Note that because we are casting pointers there can be no user-defined operators to interfere.*/

    template <typename Base, typename Derived>
    constexpr bool is_virtual_base_impl(...) { return true; }

    template <typename Base, typename Derived, std::void_t<decltype((Derived*)(std::declval<Base*>()))>* = nullptr>
    constexpr bool is_virtual_base_impl(int) { return false; }
}

template <typename Base, typename Derived>
struct is_virtual_base_of : public std::bool_constant<
    std::is_base_of<Base, Derived>::value &&
    detail::is_virtual_base_impl<Base, Derived>(0) &&
    !detail::is_virtual_base_impl<Derived, Base>(0)> {};

template <typename Base, typename Derived>
constexpr bool is_virtual_base_of_v = is_virtual_base_of<Base, Derived>::value;

// Another important trait
template <typename U, typename T>
struct is_explicitly_convertible
    : std::bool_constant<std::is_constructible_v<T, U> && !std::is_convertible_v<U, T>> {};

template <typename U, typename T>
static constexpr bool is_explicitly_convertible_v = is_explicitly_convertible<U, T>::value;

template <template<typename> typename Pred, typename... Ts>
constexpr bool all_of_pred = (Pred<Ts>::value && ...);

template <template<typename> typename Pred, typename... Ts>
constexpr bool any_of_pred = (Pred<Ts>::value && ...);
