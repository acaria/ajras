#pragma once

#include "ECS.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "OrientationComponent.h"
#include "StaminaComponent.h"
#include "ProfileData.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "MeleeComponent.h"
#include "HealthComponent.h"
#include "AIComponent.h"
#include "InteractComponent.h"
#include "TeamComponent.h"
#include "CmdComponent.h"
#include "GateMap.h"
#include "WarpMap.h"
#include "SlotData.h"
#include "TrailComponent.h"

namespace cp
{
    using GearComponent = std::list<SlotData>;
    
    using Render = ecs::component<RenderComponent,              1>;
    using Position = ecs::component<PositionComponent,          2>;
    using Orientation = ecs::component<OrientationComponent,    3>;
    using Cmd = ecs::component<CmdComponent,                    4>;
    using Stamina = ecs::component<StaminaComponent,            5>;
    using Input = ecs::component<InputComponent,                6>;
    using Physics = ecs::component<PhysicsComponent,            7>;
    using Gate = ecs::component<GateMap,                        8>;
    using Warp = ecs::component<WarpMap,                        9>;
    using Interact = ecs::component<InteractComponent,          10>;
    using Control = ecs::component<unsigned,                    11>;
    using Melee = ecs::component<MeleeComponent,                12>;
    using Health = ecs::component<HealthComponent,              13>;
    using Target = ecs::component<unsigned,                     14>;
    using AI = ecs::component<AIComponent,                      15>;
    using Gear = ecs::component<GearComponent,                  16>;
    using Collectible = ecs::component<std::string,             17>;
    using Mood = ecs::component<def::mood::Flags,               18>;
    using Team = ecs::component<TeamComponent,                  19>;
    using Trail = ecs::component<TrailComponent,                20>;
    
    struct entity
    {
        static unsigned genID(bool reset = false) {
            static unsigned eid = def::startEntityIndex;
            
            if (reset) eid = def::startEntityIndex - 1;
            return eid++;
        }
        
        static void resetID()
        {
            entity::genID(true);
        }
        
        static void move(unsigned id, unsigned g1, unsigned g2)
        {
            ecs::move<Render>(id, g1, g2);
            ecs::move<Position>(id, g1, g2);
            ecs::move<Orientation>(id, g1, g2);
            ecs::move<Cmd>(id, g1, g2);
            ecs::move<Stamina>(id, g1, g2);
            ecs::move<Input>(id, g1, g2);
            ecs::move<Physics>(id, g1, g2);
            ecs::move<Gate>(id, g1, g2);
            ecs::move<Warp>(id, g1, g2);
            ecs::move<Interact>(id, g1, g2);
            ecs::move<Control>(id, g1, g2);
            ecs::move<Melee>(id, g1, g2);
            ecs::move<Health>(id, g1, g2);
            ecs::move<Target>(id, g1, g2);
            ecs::move<AI>(id, g1, g2);
            ecs::move<Gear>(id, g1, g2);
            ecs::move<Collectible>(id, g1, g2);
            ecs::move<Mood>(id, g1, g2);
            ecs::move<Team>(id, g1, g2);
            ecs::move<Trail>(id, g1, g2);
        }
        
        static void remove(unsigned id, unsigned group)
        {
            ecs::del<Render>(id, group);
            ecs::del<Position>(id, group);
            ecs::del<Orientation>(id, group);
            ecs::del<Cmd>(id, group);
            ecs::del<Stamina>(id, group);
            ecs::del<Input>(id, 1);
            ecs::del<Physics>(id, group);
            ecs::del<Gate>(id, group);
            ecs::del<Warp>(id, group);
            ecs::del<Interact>(id, group);
            ecs::del<Control>(id, group);
            ecs::del<Melee>(id, group);
            ecs::del<Health>(id, group);
            ecs::del<Target>(id, group);
            ecs::del<AI>(id, group);
            ecs::del<Gear>(id, group);
            ecs::del<Collectible>(id, group);
            ecs::del<Mood>(id, group);
            ecs::del<Team>(id, group);
            ecs::del<Trail>(id, group);
        }
        
        static void clear(unsigned group)
        {
            ecs::clearGroup<Render>(group);
            ecs::clearGroup<Position>(group);
            ecs::clearGroup<Orientation>(group);
            ecs::clearGroup<Cmd>(group);
            ecs::clearGroup<Stamina>(group);
            ecs::clearGroup<Input>(group);
            ecs::clearGroup<Physics>(group);
            ecs::clearGroup<Gate>(group);
            ecs::clearGroup<Warp>(group);
            ecs::clearGroup<Interact>(group);
            ecs::clearGroup<Control>(group);
            ecs::clearGroup<Melee>(group);
            ecs::clearGroup<Health>(group);
            ecs::clearGroup<Target>(group);
            ecs::clearGroup<AI>(group);
            ecs::clearGroup<Gear>(group);
            ecs::clearGroup<Collectible>(group);
            ecs::clearGroup<Mood>(group);
            ecs::clearGroup<Team>(group);
            ecs::clearGroup<Trail>(group);
        }
    };
}