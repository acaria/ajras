#include "SystemFacade.h"
#include "ECSGroup.h"
#include "LayeredContainer.h"
#include "IMapData.h"

SystemFacade::SystemFacade(SystemDispatcher& dispatcher, SystemContext& context,
                           lib::Random& random): dispatcher(dispatcher),
                                                 context(context),
                                                 random(random)
{
}

void SystemFacade::tick(double dt)
{
    for(auto& system : list)
        system->tick(dt);
}

void SystemFacade::animate(double dt, double tp)
{
    for(auto& system : list)
        system->animate(dt, tp);
}