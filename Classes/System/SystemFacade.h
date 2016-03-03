#pragma once

#include "BaseSystem.h"
#include "Randgine.h"
#include "SystemContext.h"
#include "SystemDispatcher.h"

#include "CmdSystem.h"
#include "ControlSystem.h"
#include "RenderSystem.h"
#include "CollisionSystem.h"
#include "TransitSystem.h"
#include "MeleeSystem.h"
#include "TargetSystem.h"
#include "AISystem.h"
#include "TargetSystem.h"
#include "UpdaterSystem.h"
#include "MoveSystem.h"
#include "InteractSystem.h"
#include "CleanerSystem.h"
#if ECSYSTEM_DEBUG
#include "DebugSystem.h"
#endif

class SystemFacade
{
public:
    SystemFacade(SystemDispatcher& dispatcher, SystemContext& context, Randgine::CAT randCat);

    template <typename T, typename ... ARGS>
    void factory(ARGS&&... args)
    {
        static_assert(std::is_base_of<BaseSystem, T>::value, "need inheritance from BaseSystem");
        auto system = std::unique_ptr<T>(new T (std::forward<ARGS>(args)...));
        system->prepare(&this->context, &this->dispatcher, &this->random);
        list.push_back(std::move(system));
    }
    
    void tick(double dt);
    void animate(double dt, double tp);
    
private:
    SystemContext&                          context;
    SystemDispatcher&                       dispatcher;
    lib::Random&                            random;

    std::list<std::unique_ptr<BaseSystem>>  list;
};