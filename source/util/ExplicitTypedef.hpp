//--------------------------------------------------------------------------------
// ExplicitTypedef.hpp
//--------------------------------------------------------------------------------
// Provides a macro to enable explicit typedefs, that need to be instantiated.
// That can be used to provide named parameters
//--------------------------------------------------------------------------------
#pragma once

#define EXPLICIT_TYPEDEF(Name, ...) \
class Name final\
{\
    __VA_ARGS__ _val;\
public:\
    explicit constexpr Name() : _val() {}\
    explicit constexpr Name(__VA_ARGS__ v) : _val(v) {}\
    constexpr operator __VA_ARGS__() const { return _val; }\
}
