#pragma once
#include "Headers.h"

namespace cp
{
    static unsigned playerID = 0;

    using Render = ecs::component<RenderComponent, 1>;
    using Position = ecs::component<PositionComponent, 2>;
    using Orientation = ecs::component<OrientationComponent, 3>;
    using Velocity = ecs::component<VelocityComponent, 4>;
    using Profile = ecs::component<ProfileData*, 5>;
    using Input = ecs::component<InputComponent, 6>;
    using Collision = ecs::component<CollisionComponent, 7>;
    using Gate = ecs::component<GateComponent, 8>;
    using Control = ecs::component<bool, 9>;
    using Melee = ecs::component<MeleeComponent, 10>;
    
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
            ecs::move<Control>(id, g1, g2);
            ecs::move<Melee>(id, g1, g2);
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
            ecs::del<Control>(id, group);
            ecs::del<Melee>(id, group);
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
            ecs::clearGroup<Control>(group);
            ecs::clearGroup<Melee>(group);
            
            genID(true);
        }
    };
}