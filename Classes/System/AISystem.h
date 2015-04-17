#pragma once

class EcsGroup;
class RoomData;

#include "BaseTickSystem.h"
#include "BehaviourNodes.h"


class AISystem : public BaseTickSystem
{
public:
    AISystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    void init(RoomData* data);
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
    behaviour::nState onCheck(unsigned eid, unsigned nid);
    behaviour::nState onExecute(unsigned eid, unsigned nid);
    
private:
    enum class ExecBType {
        TARGET,
        MOVE_TO,
        MOVE_NEAR,
        MOVE_DIR
    };
    
    enum class CheckBType {
        TIME,
        COLLISION
    };
    
    enum class ActionBType {
        RAND,
        STOP,
        TARGET
    };
    
    std::map<std::string, CheckBType> checkMap = {
        {"time", CheckBType::TIME},
        {"collision", CheckBType::COLLISION},
    };
    
    std::map<std::string, ExecBType> execMap = {
        {"target", ExecBType::TARGET},
        {"moveto", ExecBType::MOVE_TO},
        {"movenear", ExecBType::MOVE_NEAR},
        {"movedir", ExecBType::MOVE_DIR},
    };
    
    std::map<std::string, ActionBType> actionMap = {
        {"rand", ActionBType::RAND},
        {"target", ActionBType::TARGET},
        {"stop", ActionBType::STOP}
    };
    
    RoomData* data;
};