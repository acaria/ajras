#pragma once

#include "BehaviourNodes.h"
#include "Defines.h"

class AISystem;

class AIHelper
{
public:
    AIHelper(AISystem* system);

    using Properties = std::map<std::string, cc::Value>;
    using state = behaviour::nState;

    behaviour::nState checkNearMood(unsigned eid, float sight,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState checkTime(unsigned eid, double dt,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState checkCollision(unsigned eid,
        const std::vector<std::string>& params);
    
    behaviour::nState execAnim(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execTargetMood(unsigned eid, float sight,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execTargetNone(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execTargetPlayer(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execMoveToRand(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execMoveStop(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execMoveToSleepZone(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execMoveDirRand(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execMoveDirTarget(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execMoveNearTarget(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execFollowTeam(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
    behaviour::nState execStopSleepZone(unsigned eid,
        const std::vector<std::string>& params, Properties& properties);
    
private:
    behaviour::nState followPathFinding(unsigned eid, Properties& properties, float reachGoal);
    def::mood::Flags getMoodGroup(def::mood::Flags ref, const std::string& moodGroupCat);
    AISystem* system;
};