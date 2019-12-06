#pragma once

#include <string>
#include <cstdint>

std::string toHex(std::uintmax_t v, std::intmax_t minChars = 0);
std::string simplifyPath(const std::string &str);
std::string labelizeName(const std::string &name, bool stripExtension = false);
std::string labelizeString(std::string str);

struct SpecialName
{
    std::string nmspace;
    std::string fileName;
    std::string mangledName;
};

SpecialName deriveSpecialName(const std::string& name);