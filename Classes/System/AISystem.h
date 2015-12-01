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

    enum class ExecBType {
        TARGET,
        MOVE_TO,
        MOVE_NEAR,
        MOVE_DIR,
        CHARGE,
        STOP,
        ANIM
    };
    
    enum class CheckBType {
        NEAR,
        TIME,
        COLLISION
    };
    
    enum class ActionBType {
        RAND,
        SLEEPZONE,
        STOP,
        TARGET
    };
    
    std::map<std::string, CheckBType> checkMap = {
        {"near", CheckBType::NEAR},
        {"time", CheckBType::TIME},
        {"collision", CheckBType::COLLISION},
    };
    
    std::map<std::string, ExecBType> execMap = {
        {"target", ExecBType::TARGET},
        {"moveto", ExecBType::MOVE_TO},
        {"movenear", ExecBType::MOVE_NEAR},
        {"movedir", ExecBType::MOVE_DIR},
        {"charge", ExecBType::CHARGE},
        {"stop", ExecBType::STOP},
        {"anim", ExecBType::ANIM}
    };
    
    std::map<std::string, ActionBType> actionMap = {
        {"rand", ActionBType::RAND},
        {"sleep_zone", ActionBType::SLEEPZONE},
        {"target", ActionBType::TARGET},
        {"stop", ActionBType::STOP}
    };
};