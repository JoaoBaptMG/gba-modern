#pragma once

#include <string>
#include <cstdint>

std::string toHex(std::uintmax_t v, std::intmax_t minChars = 0);
std::string simplifyPath(const std::string &str);
std::string labelizeName(const std::string &name, bool stripExtension = false);
