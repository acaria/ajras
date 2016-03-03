#include "CleanerSystem.h"
#include "Components.h"

void CleanerSystem::tick(double dt)
{
    for(auto eid : toRemove)
    {
        cp::entity::remove(eid, context->ecs->getID());
    }
    toRemove.clear();
}

void CleanerSystem::init()
{
    this->eventRegs.push_back(dispatcher->onEntityDeleted.registerObserver(
            [this](unsigned group, unsigned eid) {
        toRemove.push_back(eid);
    }));
}