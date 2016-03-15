#pragma once

namespace behaviour
{
    struct Waypoints
    {
        Waypoints(std::vector<cc::Point>& waypoints) : points(waypoints) {}
        std::vector<cc::Point> points;
    };

    using Variant = lib::variant<std::string,
                                 unsigned, int, float, double, bool,
                                 cc::Point, lib::v2u, Waypoints>;
    using Properties = std::map<std::string, Variant>;

    static inline Variant& addProperty(Properties& properties, const std::string& tag)
    {
        properties[tag] = Variant();
        return properties[tag];
    }
}

