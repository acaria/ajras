#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"

class IMapData;
class LayeredContainer;

class DebugSystem : public BaseTickSystem
{
public:
    DebugSystem(lib::EcsGroup& ecs);
    virtual ~DebugSystem();
    
    void init(LayeredContainer *view, IMapData *data);
    
    //overrides
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {};


private:

    IMapData*           data;
    LayeredContainer*   view;
    cc::Node* debugLayer = nullptr;
    
    cc::Sprite* addPixel(cc::Color3B color, const cc::Rect& bounds);
    
    //collision
    std::set<unsigned>              collisionSet;
    std::map<unsigned, cc::Sprite*> collisionMap;
};