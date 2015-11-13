#include "HealthSystem.h"
#include "Components.h"

HealthSystem::HealthSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {
    
}

void HealthSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Health, cp::Render>())
    {
        auto& cpHealth = ecs::get<cp::Health>(eid);
        auto& cpRender = ecs::get<cp::Render>(eid);
        
        if (cpHealth.damage > 0)
        {
            cpHealth.hp -= cpHealth.damage;
            cpHealth.damage = 0;
            this->onHealthChanged(eid, cpHealth.hp);
            
            if (cpHealth.hp <= 0)
            {
                if (ecs::has<cp::Input>(eid))
                    ecs::get<cp::Input>(eid).forceDisable();
                cpRender.sprite->stopAllActions();
                cpRender.sprite->setColor({255,255,255});
                cpRender.setAnimation("death", 1, [eid, this](bool cancel){
                    cp::entity::remove(eid, ecs.getID());
                });
            }
        }
    }
}