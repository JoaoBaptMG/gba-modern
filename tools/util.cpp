#include "util.hpp"

#include <vector>
#include <cctype>
#include <algorithm>

std::string simplifyPath(const std::string &str)
{
    // Store the directory's segments
    std::vector<std::string> segments;
    std::size_t numPrevsInBeginning = 0;

    // Now, pass through each segment
    std::size_t curPos = std::string::npos;
    do
    {
        std::size_t prevPos = curPos+1, size;
        curPos = str.find('/', prevPos);
        if (curPos == std::string::npos) size = curPos;
        else size = curPos - prevPos;
        auto curSegment = str.substr(prevPos, size);

        // Check for the special "." and ".." segment
        if (curSegment == "..")
        {
            if (segments.empty()) numPrevsInBeginning++;
            else segments.pop_back();
        }
        else if (curSegment != ".")
            segments.push_back(curSegment);

    } while (curPos != std::string::npos);

    // Now, compose the simplified path
    std::string result;
    for (std::size_t i = 0; i < numPrevsInBeginning; i++)
        result += "../";
    for (const auto& segment : segments)
        result += segment + '/';
    result.pop_back();

    return result;
}

std::string labelizeName(const std::string &name, bool stripExtension)
{
    auto shortName = name.substr(name.find_last_of('/') + 1);
    if (stripExtension)
        shortName = shortName.substr(0, shortName.find_last_of('.'));
    
    for (auto& c : shortName) if (!std::isalnum(c)) c = '_';
    return shortName;
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