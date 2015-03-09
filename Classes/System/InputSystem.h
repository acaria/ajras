#pragma once
#include "Headers.h"

class InputSystem : public BaseTickSystem
{
public:
    InputSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    ~InputSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
private:
    bool checkPredicates(unsigned eid, InputComponent& cp);
};