#include "util.hpp"
#include "split.hpp"

#include <vector>
#include <cctype>
#include <algorithm>

std::string simplifyPath(const std::string &str)
{
    // Store the directory's segments
    std::vector<std::string> segments;
    std::size_t numPrevsInBeginning = 0;

    // Now, pass through each segment
    for (auto curSegment : split(str, '/'))
    {
        // Check for the special "." and ".." segment
        if (curSegment == "..")
        {
            if (segments.empty()) numPrevsInBeginning++;
            else segments.pop_back();
        }
        else if (curSegment != ".") segments.emplace_back(curSegment);
    }

    // Now, compose the simplified path
    std::string result;
    for (std::size_t i = 0; i < numPrevsInBeginning; i++)
        result += "../";
    for (const auto& segment : segments)
        result += std::string(segment) + '/';
    result.pop_back();

    return result;
}

std::string labelizeName(const std::string &name, bool stripExtension)
{
    auto shortName = name.substr(name.find_last_of('/') + 1);
    if (stripExtension)
        shortName = shortName.substr(0, shortName.find_last_of('.'));
    
    return labelizeString(shortName);
}

std::string labelizeString(std::string str)
{
    for (auto& c : str)
        if (!std::isalnum(c)) c = '_';
    return str;
}

std::string toHex(std::uintmax_t v, std::intmax_t minChars)
{
    static const char* digits = "0123456789ABCDEF";

    std::string str;

    while (minChars-- > 0 || v)
    {
        auto digit = v & 0xF;
        str.push_back(digits[digit]);
        v >>= 4;
    }

    str += "x0";
    std::reverse(str.begin(), str.end());
    return str;
}

SpecialName deriveSpecialName(const std::string& name)
{
    SpecialName result;

    std::vector<std::string> splitVector;
    for (auto sv : split(simplifyPath(name), '/'))
        splitVector.emplace_back(sv);

    // build the filename
    result.fileName = splitVector.back();
    result.fileName = result.fileName.substr(0, result.fileName.find_last_of('.'));
    result.fileName = labelizeString(result.fileName);
    splitVector.pop_back();

    // now, build the namespace and the mangled name
    bool nsnotfirst = false;
    result.mangledName = "_ZN";
    for (const auto& str : splitVector)
    {
        auto lstr = labelizeString(str);
        if (nsnotfirst) result.nmspace += "::";
        nsnotfirst = true;
        result.nmspace += lstr;
        result.mangledName += std::to_string(lstr.size()) + lstr;
    }
    // Add the last file name and the mangling details
    result.mangledName += std::to_string(result.fileName.size()) + result.fileName + "E";

    return result;
}
