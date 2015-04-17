#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"
class RoomData;

class RenderSystem : public BaseTickSystem
{
public:
    RenderSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    ~RenderSystem() {}
    
    void init(RoomData* data);
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final;
    
private:
    RoomData* data;
};