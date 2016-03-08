#include "AIHelper.h"
#include "AISystem.h"
#include "RoomData.h"
#include "GameCtrl.h"

AIHelper::AIHelper(AISystem* system) : system(system) {}

behaviour::nState AIHelper::checkNearMood(unsigned eid, float sight,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 2); //params=[category,value]
    
    auto maxDist = sight * sight;
    auto moodGroup = getMoodGroup(ecs::get<cp::Mood>(eid), params[1]);
    auto bounds = SysHelper::getBounds(eid);
    
    float nearest = maxDist;
    unsigned targetID = 0;

    for(auto tid : system->context->ecs->join<cp::AI, cp::Position, cp::Physics, cp::Mood>())
    {
        if (tid == eid)
            continue;
        if (!def::mood::inside(ecs::get<cp::Mood>(tid), moodGroup))
            continue;
        auto bounds2 = SysHelper::getBounds(tid);
        float dist = cc::Vec2(bounds.getMidX() - bounds2.getMidX(),
                              bounds.getMidY() - bounds2.getMidY()).lengthSquared();
        if (dist < maxDist && dist < nearest)
        {
            nearest = dist;
            targetID = tid;
        }
    }

    return (targetID != 0) ? state::SUCCESS : state::FAILURE;
}

behaviour::nState AIHelper::checkTime(unsigned eid, double dt,
                                      const std::vector<std::string>& params,
                                      Properties& properties)
{
    assert(params.size() == 1); //params=[timer]
    if (!lib::hasKey(properties, "timer"))
        properties["timer"] = std::stod(params[0]);
    if (properties["timer"].asDouble() <= 0)
    {
        //timeout
        properties.erase("timer");
        return state::FAILURE;
    }
    properties["timer"] = properties["timer"].asDouble() - dt;
    return state::SUCCESS;
}

behaviour::nState AIHelper::checkCollision(unsigned eid, const std::vector<std::string>& params)
{
    assert(params.size() == 0); //no params
    if (!ecs::has<cp::Physics>(eid) ||
        ecs::get<cp::Physics>(eid).collisionState != PhysicsComponent::CollisionType::NONE)
        return state::FAILURE;
    return state::SUCCESS;
}

behaviour::nState AIHelper::execAnim(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    if (!ecs::has<cp::Render>(eid))
        return state::FAILURE;
    
    if (!lib::hasKey(properties, "done"))
    {
        properties["done"] = false;
        assert(params.size() > 0); // params=[keyName, repeat]
        std::string keyName = params[0];
        int repeat = -1;
        if (params.size() > 1)
            repeat = std::stoi(params[1]);
        ecs::get<cp::Render>(eid).setAnimation(keyName, repeat, [&properties](bool){
            properties["done"] = true;
        });
    }
    return properties["done"].asBool() ? state::SUCCESS : state::RUNNING;
}

behaviour::nState AIHelper::execTargetMood(unsigned eid, float sight,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 2); //params=[category,value]
    
    auto moodGroup = getMoodGroup(ecs::get<cp::Mood>(eid), params[1]);
    auto targetID = SysHelper::getNearest(system->context->ecs->getID(), eid, moodGroup, sight);
    if (targetID != nullptr)
    {
        system->context->ecs->add<cp::Target>(eid) = targetID.Value;
        return state::SUCCESS;
    }
    return state::FAILURE;
}

behaviour::nState AIHelper::execTargetPlayer(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    auto focusID = GameCtrl::instance()->getData().getPlayerData()->getEntityFocusID();
    system->context->ecs->add<cp::Target>(eid) = focusID;
    return state::SUCCESS;
}

behaviour::nState AIHelper::execTargetNone(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    system->context->ecs->del<cp::Target>(eid);
    return state::SUCCESS;

}

behaviour::nState AIHelper::execMoveToRand(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    
    if (!ecs::has<cp::Input, cp::Physics>(eid))
        return state::FAILURE;
    
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    
    if (!lib::hasKey(properties, "waypoints"))
    {
        auto originBounds = SysHelper::getBounds(eid);
        auto coords = system->context->data->getCol()->getAllStaticFreeCoords(cpPhy.category);
        
        do
        {
            auto coord = system->random->pick(/*ref*/coords);
            //convert coords to positions
            auto destBounds = system->context->data->getBlockBound(coord);
            
            auto wayPoints = system->context->data->getNav()->getWaypoints(
                {eid}, originBounds,
                {destBounds.getMidX(), destBounds.getMidY()},
                cpPhy.category);
            
            if (wayPoints.size() > 0)
            {
                ecs::get<cp::Debug>(eid).wayPoints = wayPoints;

                cc::ValueVector wayPointsVec = linq::from(wayPoints) >>
                    linq::select([](cc::Point coord) {
                        return cc::Value(cc::ValueMap{{"x", cc::Value((int)coord.x)},
                                                      {"y", cc::Value((int)coord.y)}});
                }) >> linq::to_vector();

                properties["waypoints"] = wayPointsVec;
                break;
            }
        }
        while(coords.size() > 0);
        
        if (!lib::hasKey(properties, "waypoints"))
            return state::FAILURE;
    }
    
    return this->followPathFinding(eid, properties, 4);
}

behaviour::nState AIHelper::execMoveStop(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    if (ecs::has<cp::Input>(eid))
        ecs::get<cp::Input>(eid).direction = cc::Vec2::ZERO;
    return state::SUCCESS;
}

behaviour::nState AIHelper::execMoveToSleepZone(unsigned eid,
    const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    //dirty cast
    auto sleepZoneData = dynamic_cast<ISleepZones*>(system->context->data);
    if (sleepZoneData)
    {
        if (!ecs::has<cp::Input>(eid))
            return state::FAILURE;
        if (!lib::hasKey(properties, "target"))
        {
            if (!ecs::has<cp::AI, cp::Position, cp::Physics>(eid))
                return state::FAILURE;
            auto bounds = SysHelper::getBounds(eid);
            auto sleepZone = sleepZoneData->getSleepZone(ecs::get<cp::AI>(eid).sleep,
                                                         {bounds.getMidX(), bounds.getMidY()});
            if (sleepZone == nullptr)
                return state::FAILURE;
            sleepZone->taken = true;
            auto& shape = ecs::get<cp::Physics>(eid).shape;
            cc::Point pos = {
                (sleepZone->bounds.getMidX() - bounds.size.width / 2) - shape.origin.x,
                (sleepZone->bounds.getMidY() - bounds.size.height / 2) - shape.origin.y
            };
            properties["target"] = cc::ValueMap{{"x", cc::Value((int)pos.x)},
                                                {"y", cc::Value((int)pos.y)}};
        }
        
        auto& cpInput = ecs::get<cp::Input>(eid);
        auto currentPos = ecs::get<cp::Position>(eid).pos;
        auto vdir = cc::Vec2(properties["target"].asValueMap()["x"].asInt() - currentPos.x,
                             properties["target"].asValueMap()["y"].asInt() - currentPos.y);
        if (vdir.length() < 5.0)
        {
            return state::SUCCESS;
        }
        cpInput.direction = vdir.getNormalized() * MIN(1.0, vdir.length() / 5);
        return state::RUNNING;
    }
    return state::FAILURE;
}

behaviour::nState AIHelper::execFollowTeam(unsigned eid,
        const std::vector<std::string>& params, Properties& properties)
{
    const float minDistFollow = 30.0;
    const float minKeepDistance = 20.0;
    
    assert(params.size() == 1); //params=[category]
    
    auto gid = system->context->ecs->getID();
    
    if (!ecs::has<cp::Position, cp::Physics>(eid))
        return state::FAILURE;
    
    //if too close from target, stop following
    if ((properties.find("leaderId") != properties.end()))
    {
        auto leaderId = properties["leaderId"].asInt();
        if (!ecs::has<cp::Position, cp::Physics>(leaderId))
            return state::FAILURE;
        if (SysHelper::getDistSquared(eid, leaderId) < (minDistFollow * minDistFollow))
        {
            auto teamIds = SysHelper::findTeamIds(gid, ecs::get<cp::Team>(eid).index);
            return this->keepTeamDistance(eid, leaderId, teamIds, minKeepDistance);
        }
    }

    //path finding in progress
    if (properties.find("waypoints") != properties.end())
    {
        auto state = this->followPathFinding(eid, properties, 2);
        
        //keep distance between team entities
        auto leaderId = properties["leaderId"].asInt();
        if (state == state::RUNNING && ecs::has<cp::Team>(eid) && eid != leaderId)
        {
            auto teamIds = SysHelper::findTeamIds(gid, ecs::get<cp::Team>(eid).index);
            this->keepTeamDistance(eid, leaderId, teamIds, minKeepDistance);
        }
        
        return state;
    }
    
    //compute team info
    if (!ecs::has<cp::Team>(eid))
        return state::FAILURE;
    auto& cpTeam = ecs::get<cp::Team>(eid);
    
    if (cpTeam.position == 0) //you are the leader, no follow
        return state::SUCCESS;
    
    unsigned leaderId = SysHelper::findTeamLeaderId(gid, eid, cpTeam.index);
    if (leaderId == 0)
        return state::FAILURE;
    
    if (!ecs::has<cp::Trail>(leaderId) || ecs::get<cp::Trail>(leaderId).tail.size() == 0)
        return state::FAILURE;
    
    properties["leaderId"] = (int)leaderId;
    properties["followedId"] = (int)leaderId;
    
    if (SysHelper::getDistSquared(eid, leaderId) < minDistFollow)
    {
        //too close, stop follow
        return state::SUCCESS;
    }
    
    auto bounds = SysHelper::getBounds(eid);
    cc::Point dest = system->context->data->getCol()->getFormationPosition(
        cpTeam.formation, cpTeam.position, ecs::get<cp::Trail>(leaderId).tail);
    auto wayPoints = system->context->data->getNav()->getWaypoints(
            {eid, leaderId}, bounds, dest, ecs::get<cp::Physics>(eid).category);
    
    if (wayPoints.size() > 0)
    {
        ecs::get<cp::Debug>(eid).wayPoints = wayPoints;
        cc::ValueVector wayPointsVec = linq::from(wayPoints) >>
                linq::select([](cc::Point coord) {
            return cc::Value(cc::ValueMap{{"x", cc::Value((int)coord.x)},
                                          {"y", cc::Value((int)coord.y)}});
        }) >> linq::to_vector();
        
        properties["waypoints"] = wayPointsVec;
        return state::RUNNING;
    }
    
    return state::FAILURE;
}

behaviour::nState AIHelper::execMoveNearTarget(unsigned eid,
        const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 2); //params=[category, range]
    float range = std::stod(params[1]);
    
    if (!ecs::has<cp::Target, cp::Input>(eid))
        return state::FAILURE;
    auto tid = ecs::get<cp::Target>(eid);
    if (!ecs::has<cp::Position, cp::Physics>(tid))
        return state::FAILURE;
    
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    
    auto bounds = SysHelper::getBounds(eid);
    auto bounds2 = SysHelper::getBounds(tid);
    
    if (properties.find("waypoints") != properties.end())
    {
        return this->followPathFinding(eid, properties, range);
    }
    
    auto wayPoints = system->context->data->getNav()->getWaypoints({eid},
            bounds, {bounds2.getMidX(), bounds2.getMidY()}, cpPhy.category);
            
    if (wayPoints.size() > 0)
    {
        ecs::get<cp::Debug>(eid).wayPoints = wayPoints;
        cc::ValueVector wayPointsVec = linq::from(wayPoints) >>
            linq::select([](cc::Point coord) {
                return cc::Value(cc::ValueMap{{"x", cc::Value((int)coord.x)},
                                              {"y", cc::Value((int)coord.y)}});
            }) >> linq::to_vector();
                
            properties["waypoints"] = wayPointsVec;
            return state::RUNNING;
    }
    return state::FAILURE;
}

behaviour::nState AIHelper::execMoveDirTarget(unsigned eid,
        const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 2); //params=[category, range]
    float range = std::stod(params[1]);
    
    if (!ecs::has<cp::Target, cp::Input>(eid))
        return state::FAILURE;
    auto tid = ecs::get<cp::Target>(eid);
    if (!ecs::has<cp::Position, cp::Physics>(tid))
        return state::FAILURE;
    
    auto bounds = SysHelper::getBounds(eid);
    auto bounds2 = SysHelper::getBounds(tid);
    
    auto& cpInput = ecs::get<cp::Input>(eid);
    auto vdir = cc::Vec2(bounds2.getMidX() - bounds.getMidX(), bounds2.getMidY() - bounds.getMidY());
    if (vdir.length() < range)
        return state::SUCCESS;

    cpInput.direction = vdir.getNormalized();
    return state::RUNNING;
}

behaviour::nState AIHelper::execStopSleepZone(unsigned eid,
        const std::vector<std::string>& params, Properties& properties)
{
    assert(params.size() == 1); //params=[category]
    auto& cpAI = ecs::get<cp::AI>(eid);
    auto sleepZonesData = dynamic_cast<ISleepZones*>(system->context->data);
    if (sleepZonesData)
    {
        if (!ecs::has<cp::Position, cp::Physics>(eid))
            return state::FAILURE;
        auto bounds = SysHelper::getBounds(eid);
        sleepZonesData->freeSleepZone(cpAI.sleep, {bounds.getMidX(), bounds.getMidY()});
    }
    return state::SUCCESS;
}

def::mood::Flags AIHelper::getMoodGroup(def::mood::Flags ref,
                                        const std::string& moodGroupCat)
{
    switch(lib::hash(moodGroupCat))
    {
        case lib::hash("opponent"): return def::mood::getOpponents(ref);
        case lib::hash("ally"): return def::mood::getAllies(ref);
        default:
            Log("unrecognised mood group selection : %s", moodGroupCat.c_str());
            break;
    }
    return def::mood::Neutral;
}

behaviour::nState AIHelper::followPathFinding(unsigned eid, Properties& properties, float reachGoal)
{
    assert(properties.find("waypoints") != properties.end());
    auto& wayPoints = properties["waypoints"].asValueVector();
    
    if (wayPoints.size() == 0)
    {
        ecs::get<cp::Debug>(eid).wayPoints.clear();
        ecs::get<cp::Input>(eid).direction = {0,0};
        return state::SUCCESS;
    }
    
    if (!ecs::get<cp::Input>(eid).enabled)
    {
        ecs::get<cp::Debug>(eid).wayPoints.clear();
        return state::FAILURE;
    }
    
    auto& vMap = wayPoints.front().asValueMap();
    cc::Point destPos { vMap["x"].asFloat(), vMap["y"].asFloat() };
    
    auto bounds = SysHelper::getBounds(eid);
    
    if (properties.find("followedId") != properties.end())
    {
        auto& vMap2 = wayPoints.back().asValueMap();
        cc::Point lastPos { vMap2["x"].asFloat(), vMap2["y"].asFloat() };
        auto fBounds = SysHelper::getBounds(properties["followedId"].asInt());
        auto prevCoord = system->context->data->getCoordFromPos(lastPos);
        auto newCoord = system->context->data->getCoordFromPos({fBounds.getMidX(),
                                                                fBounds.getMidY()});
        if (prevCoord != newCoord)
        {
            auto tileSize = system->context->data->getTileSize();
            cc::Point newPos = { newCoord.x * tileSize.width + tileSize.width / 2,
            newCoord.y * tileSize.height + tileSize.height / 2};
            //add a step to reach new pos of followed entity
            wayPoints.push_back(cc::Value(cc::ValueMap{
                {"x", cc::Value((int)newPos.x)},
                {"y", cc::Value((int)newPos.y)}}));
        }
    }
    
    cc::Vec2 vdir { destPos.x - bounds.getMidX(), destPos.y - bounds.getMidY() };
    
    if (vdir.length() <= reachGoal)
    {
        wayPoints.erase(wayPoints.begin());
        
        std::list<cc::Point> wDebug;
        for (auto step : wayPoints)
        {
            auto& vMap = step.asValueMap();
            cc::Point stepPos { vMap["x"].asFloat(), vMap["y"].asFloat() };
            wDebug.push_back(stepPos);
        }
        ecs::get<cp::Debug>(eid).wayPoints = wDebug;
    }
    else
    {
        ecs::get<cp::Input>(eid).direction = vdir.getNormalized() * MIN(1.0, vdir.length() / 5);
    }
    
    return state::RUNNING;
}

behaviour::nState AIHelper::keepTeamDistance(unsigned eid, unsigned leaderId,
                                             const std::set<unsigned>& teamIds, float distance)
{
    if (!ecs::has<cp::Team>(eid) || eid == leaderId)
        return state::SUCCESS;
    
    state resultState = state::SUCCESS;
    for(auto teamId : teamIds)
    {
        if (teamId == eid) continue;
        
        if (!ecs::has<cp::Position, cp::Physics, cp::Team>(eid) ||
            !ecs::has<cp::Position, cp::Physics, cp::Team>(teamId))
            return state::FAILURE;
        auto bounds1 = SysHelper::getBounds(eid);
        auto bounds2 = SysHelper::getBounds(teamId);
        
        auto diff = cc::Point(bounds2.getMidX() - bounds1.getMidX(),
                              bounds2.getMidY() - bounds1.getMidY());
        auto length = diff.getLength();
        if (length < distance)
        {
            cc::Vec2 unit = diff.getNormalized();
            ecs::get<cp::Physics>(eid).addImpact(length, 3.0, -unit, 0.15);
            if (teamId != leaderId)
                ecs::get<cp::Physics>(teamId).addImpact(length, 3.0, unit, 0.15);
            resultState = state::RUNNING;
        }
    }
    
    return resultState;
}