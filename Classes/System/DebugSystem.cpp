#include "DebugSystem.h"
#include "IMapData.h"

DebugSystem::DebugSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {
    
}

void DebugSystem::tick(double dt)
{
    
}

void DebugSystem::init(cc::Node *view, IMapData *data)
{
    this->data = data;
    
    this->debugLayer = cc::LayerColor::create(cc::Color4B(0, 255, 0, 50),
                                              data->getBounds().size.width,
                                              data->getBounds().size.height);
    view->addChild(debugLayer);
}