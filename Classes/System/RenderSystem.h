#pragma once
#include "BaseSystem.h"
#include "ECSGroup.h"
class IMapData;

class RenderSystem : public BaseSystem
{
public:
    ~RenderSystem() {}
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final;
};