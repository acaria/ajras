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
        properties[tag] = Variant();
        return properties[tag];
    }
}

