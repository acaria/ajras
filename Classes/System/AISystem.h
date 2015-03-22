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
        IN_SIGHT,
        TIME
    };
    
    std::map<std::string, CheckBType> checkMap = {
        {"insight", CheckBType::IN_SIGHT},
        {"time", CheckBType::TIME},
    };
    
    std::map<std::string, ExecBType> execMap = {
        {"target", ExecBType::TARGET},
        {"movenear", ExecBType::MOVE_NEAR},
        {"movenear", ExecBType::MOVE_DIR},
    };
};