#pragma once

class EcsGroup;
class IMapData;

#include "BaseSystem.h"
#include "BehaviourNodes.h"
#include "Defines.h"
#include "AIHelper.h"

class AISystem : public BaseSystem
{
    friend AIHelper;
public:
    AISystem() : cmd(this)
    {}
    
    void init() override;
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
    behaviour::nState onCheck(unsigned eid, unsigned nid, double dt);
    behaviour::nState onExecute(unsigned eid, unsigned nid, double dt);
    
private:
    AIHelper cmd;
    behaviour::Properties gBoard;
    bool enabled = false;
};