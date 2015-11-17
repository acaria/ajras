#pragma once
#include "BaseSystem.h"
#include "ECSGroup.h"

class IMapData;
class LayeredContainer;

class DebugSystem : public BaseSystem
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
    cc::Label* addText(cc::Color3B color, const cc::Rect& bounds, const std::string& txt);
    
    template<class T>
    void purge(const std::set<unsigned>& ref, std::set<unsigned>& destSet, std::map<unsigned, T>& destMap);
    
    //collision
    std::set<unsigned>              collisionSet;
    std::map<unsigned, cc::Sprite*> collisionMap;
    
    //melee
    std::set<unsigned>              meleeSet;
    std::map<unsigned, cc::Sprite*> meleeMap;
    
    //health
    std::set<unsigned>              healthSet;
    std::map<unsigned, cc::Label*>  healthMap;
    
    //AI
    std::set<unsigned>              aiSet;
    std::map<unsigned, cc::Label*>  aiMap;
};