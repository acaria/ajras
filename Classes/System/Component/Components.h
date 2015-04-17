#pragma once

#include "ECS.h"
#include "RenderComponent.h"
#include "OrientationComponent.h"
#include "PositionComponent.h"
#include "VelocityComponent.h"
#include "ProfileData.h"
#include "InputComponent.h"
#include "CollisionComponent.h"
#include "GateComponent.h"
#include "WarpComponent.h"
#include "MeleeComponent.h"
#include "HealthComponent.h"
#include "AIComponent.h"
#include "CategoryComponent.h"

namespace cp
{
    using Render = ecs::component<RenderComponent,              1>;
    using Position = ecs::component<PositionComponent,          2>;
    using Orientation = ecs::component<OrientationComponent,    3>;
    using Velocity = ecs::component<VelocityComponent,          4>;
    using Profile = ecs::component<ProfileData*,                5>;
    using Input = ecs::component<InputComponent,                6>;
    using Collision = ecs::component<CollisionComponent,        7>;
    using Gate = ecs::component<GateComponent,                  8>;
    using Warp = ecs::component<WarpComponent,                  9>;
    using Control = ecs::component<unsigned,                    10>;
    using Melee = ecs::component<MeleeComponent,                11>;
    using Health = ecs::component<HealthComponent,              12>;
    using Target = ecs::component<unsigned,                     13>;
    using AI = ecs::component<AIComponent,                      14>;
    using Cat = ecs::component<CategoryComponent,               15>;
    
    struct entity
    {
        static unsigned genID(bool reset = false) {
            static unsigned eid = 1;
            
            if (reset) eid = 0;
            return eid++;
        }
        
        static void move(unsigned id, unsigned g1, unsigned g2)
        {
            ecs::move<Render>(id, g1, g2);
            ecs::move<Position>(id, g1, g2);
            ecs::move<Orientation>(id, g1, g2);
            ecs::move<Velocity>(id, g1, g2);
            ecs::move<Profile>(id, g1, g2);
            ecs::move<Input>(id, g1, g2);
            ecs::move<Collision>(id, g1, g2);
            ecs::move<Gate>(id, g1, g2);
            ecs::move<Warp>(id, g1, g2);
            ecs::move<Control>(id, g1, g2);
            ecs::move<Melee>(id, g1, g2);
            ecs::move<Health>(id, g1, g2);
            ecs::move<Target>(id, g1, g2);
            ecs::move<AI>(id, g1, g2);
            ecs::move<Cat>(id, g1, g2);
        }
        
        static void remove(unsigned id, unsigned group)
        {
            ecs::del<Render>(id, group);
            ecs::del<Position>(id, group);
            ecs::del<Orientation>(id, group);
            ecs::del<Velocity>(id, group);
            ecs::del<Profile>(id, group);
            ecs::del<Input>(id, 1);
            ecs::del<Collision>(id, group);
            ecs::del<Gate>(id, group);
            ecs::del<Warp>(id, group);
            ecs::del<Control>(id, group);
            ecs::del<Melee>(id, group);
            ecs::del<Health>(id, group);
            ecs::del<Target>(id, group);
            ecs::del<AI>(id, group);
            ecs::del<Cat>(id, group);
        }
        
        static void clear(unsigned group)
        {
            ecs::clearGroup<Render>(group);
            ecs::clearGroup<Position>(group);
            ecs::clearGroup<Orientation>(group);
            ecs::clearGroup<Velocity>(group);
            ecs::clearGroup<Profile>(group);
            ecs::clearGroup<Input>(group);
            ecs::clearGroup<Collision>(group);
            ecs::clearGroup<Gate>(group);
            ecs::clearGroup<Warp>(group);
            ecs::clearGroup<Control>(group);
            ecs::clearGroup<Melee>(group);
            ecs::clearGroup<Health>(group);
            ecs::clearGroup<Target>(group);
            ecs::clearGroup<AI>(group);
            ecs::clearGroup<Cat>(group);
            
            genID(true);
        }
    };
}