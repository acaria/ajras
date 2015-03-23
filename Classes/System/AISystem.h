#pragma once
#include "Headers.h"

class AISystem : public BaseTickSystem
{
public:
    AISystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
    behaviour::nState onCheck(unsigned eid, unsigned nid);
    behaviour::nState onExecute(unsigned eid, unsigned nid);
    
private:
    enum class ExecBType {
        TARGET,
        MOVE_NEAR,
        MOVE_DIR
    };
    
    enum class CheckBType {
        TIME
    };
    
    enum class ActionBType {
        RAND,
        STOP,
        TARGET
    };
    
    std::map<std::string, CheckBType> checkMap = {
        {"time", CheckBType::TIME},
    };
    
    std::map<std::string, ExecBType> execMap = {
        {"target", ExecBType::TARGET},
        {"movenear", ExecBType::MOVE_NEAR},
        {"movedir", ExecBType::MOVE_DIR},
    };
    
    std::map<std::string, ActionBType> actionMap = {
        {"rand", ActionBType::RAND},
        {"target", ActionBType::TARGET},
        {"stop", ActionBType::STOP}
    };
};