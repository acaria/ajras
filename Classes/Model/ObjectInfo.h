#pragma once

struct ObjectInfo
{
    cc::Vec2                            pos;
    cc::Size                            size;
    std::string                         name;
    std::string                         type;
    std::map<std::string, std::string>  properties;
};