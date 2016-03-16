#pragma once

namespace behaviour
{
    using Waypoints = std::vector<cc::Point>;
    using TeamIds = std::set<unsigned>;
    using FollowTeamInfo = std::pair<unsigned, TeamIds>;

    using Variant = lib::variant<std::string, unsigned, int, float, double, bool,
                                 cc::Point, lib::v2u,
                                 Waypoints, FollowTeamInfo, TeamIds>;
    using Properties = std::map<std::string, Variant>;

    static inline Variant& addProperty(Properties& properties, const std::string& tag)
    {
        if (properties.find(tag) == properties.end())
            properties[tag] = Variant();
        return properties[tag];
    }
    
    static inline bool checkProperty(Properties& properties, const std::string& tag)
    {
        return properties.find(tag) != properties.end();
    }
}

