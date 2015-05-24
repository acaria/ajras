#include "StringExt.h"

std::string& lib::trim(std::string& str)
{
    return ltrim(rtrim(str));
}

std::string& lib::ltrim(std::string& str)
{
    auto it2 = std::find_if(str.begin(), str.end(), [](char ch) {
        return !std::isspace<char>(ch, std::locale::classic());
    });
    str.erase(str.begin(), it2);
    return str;
}

std::string& lib::rtrim(std::string& str)
{
    auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) {
        return !std::isspace<char>(ch, std::locale::classic());
    });
    str.erase(str.begin() + (str.rend() - it1), str.end());
    return str;
}

std::string lib::trimCopy(std::string const& str)
{
    auto s = str;
    return ltrim(rtrim(s));
}