#pragma once

class EcsGroup;
class IMapData;

#include "BaseSystem.h"
#include "BehaviourNodes.h"
#include "Defines.h"

class AISystem : public BaseSystem
{
public:
    AISystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
    behaviour::nState onCheck(unsigned eid, unsigned nid, double dt);
    behaviour::nState onExecute(unsigned eid, unsigned nid, double dt);
    
private:
    def::mood::Flags getMoodGroup(def::mood::Flags ref,
                                  const std::string& moodGroupCat);
};