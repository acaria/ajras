#include "CollisionSystem.h"
#include "SysHelper.h"
#include "ModelProvider.h"
#include "IMapData.h"

CollisionSystem::~CollisionSystem()
{
}

void CollisionSystem::init()
{
    this->eventRegs.push_back(this->dispatcher->onContextChanged.registerObserver(
            [this](){
        this->sysRegs.clear();
        
        this->collisionData = context->data->getCol();
                
        this->sysRegs.push_back(this->collisionData->onDecorCollision.registerObserver(
                [this](unsigned eid, cc::Vec2 diff) {
            this->onDecorCollision(eid, diff);
        }));
        this->sysRegs.push_back(this->collisionData->onAgentCollision.registerObserver(
                [this](unsigned eid,unsigned tid, cc::Vec2 diff) {
            this->onAgentCollision(eid, tid, diff);
        }));
    }));
}

void CollisionSystem::onDecorCollision(unsigned eid, cc::Vec2 diff)
{
    auto& cpPos = ecs::get<cp::Position>(eid);
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    
    cpPhy.collisionState = PhysicsComponent::DECOR;
    cpPos.pos -= diff;
}

void CollisionSystem::onAgentCollision(unsigned eid, unsigned tid, cc::Vec2 diff)
{
    auto& cpPos = ecs::get<cp::Position>(eid);
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    
    if (collisionData->agents[tid].category == def::collision::Cat::collectible)
    {
        if (ecs::has<cp::Gear>(eid) && ecs::has<cp::Collectible>(tid))
        {
            auto collectible = ModelProvider::instance()->collectible.get(ecs::get<cp::Collectible>(tid));
            auto& cpGear = ecs::get<cp::Gear>(eid);
            if (SlotData::checkFreeSlot(cpGear, collectible))
            {
                SlotData::addCollectible(cpGear, collectible);
                ecs::get<cp::Render>(tid).sprite->removeFromParent();
                cp::entity::remove(tid, context->ecs->getID());
                this->onGearChanged(eid, cpGear);
            }
        }
    }
    else //obstacle
    {
        cpPhy.collisionState = PhysicsComponent::OBJECT;
        cpPos.pos += diff;
        
        if (ecs::get<cp::Physics>(tid).move.speed > 0)
        {
            ecs::get<cp::Position>(tid).pos -= diff;
        }
    }
}

void CollisionSystem::tick(double dt)
{
    this->collisionData->process();
}