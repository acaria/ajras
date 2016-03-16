#pragma once

#include "BehaviourNodes.h"
#include "Defines.h"
#include "ValueEx.h"
#include "Variant.h"

using namespace behaviour;

class AISystem;

class AIHelper
{
public:

    AIHelper(AISystem* system);

    nState checkNearMood(unsigned eid, float sight,
        const std::vector<std::string>& params, Properties& properties);
    
    nState checkTime(unsigned eid, double dt,
        const std::vector<std::string>& params, Properties& properties);
    
    nState checkCollision(unsigned eid,
        const std::vector<std::string>& params);
    
    nState execAnim(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    nState execTargetMood(unsigned eid, float sight,
        const std::vector<std::string>& params, Properties& properties);
    
    nState execTargetNone(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    nState execTargetPlayer(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    nState execMoveToRand(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    nState execMoveStop(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    nState execMoveToSleepZone(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    nState execMoveDirTarget(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    nState execMoveNearTarget(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    nState execFollowTeam(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    nState execStopSleepZone(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
private:
    nState followPathFinding(unsigned eid, Properties& properties, float reachGoal);
    void updatePathFinding(unsigned eid, unsigned fid, std::vector<cc::Point>& wayPoints);
    nState procTeamDistance(unsigned eid, const std::set<unsigned>& teamIds, float distance);
    nState keepTeamDistance(unsigned eid, const std::set<unsigned>& teamIds, float distance);
    def::mood::Flags getMoodGroup(def::mood::Flags ref, const std::string& moodGroupCat);
    AISystem* system;
};