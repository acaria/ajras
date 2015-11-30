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
        
        this->collisionData->agents.clear();
        
        //init agents
        for(auto eid : context->ecs->join<cp::Position, cp::Physics>())
        {
            this->collisionData->agents[eid] = SysHelper::makeAgent(eid);
        }
                
        this->sysRegs.push_back(this->collisionData->onDecorCollision.registerObserver(
                [this](unsigned eid, cc::Vec2 diff) {
            this->onDecorCollision(eid, diff);
        }));

        this->sysRegs.push_back(this->collisionData->onAgentCollision.registerObserver(
                [this](unsigned eid,unsigned tid, cc::Vec2 diff) {
            this->onAgentCollision(eid, tid, diff);
        }));
    }));
    
    this->eventRegs.push_back(this->dispatcher->onEntityAdded.registerObserver(
            [this](unsigned group, unsigned eid){
        if (context->ecs->getID() != group)
            return;
        if (ecs::has<cp::Position, cp::Physics>(eid))
            collisionData->agents[eid] = SysHelper::makeAgent(eid);
    }));
    
    this->eventRegs.push_back(this->dispatcher->onEntityDeleted.registerObserver(
            [this](unsigned group, unsigned eid) {
        collisionData->agents.erase(eid);
    }));
    
    this->eventRegs.push_back(this->dispatcher->onEntityPositionChanged.registerObserver(
            [this](unsigned group, unsigned eid) {
        if(context->ecs->getID() != group)
            return;
        if (!ecs::has<cp::Position, cp::Physics>(eid))
        {
            collisionData->agents.erase(eid);
            return;
        }
        this->collisionData->agents[eid] = SysHelper::makeAgent(eid);
    }));
}

void CollisionSystem::onDecorCollision(unsigned eid, cc::Vec2 diff)
{
    auto& cpPos = ecs::get<cp::Position>(eid);
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    
    cpPhy.collisionState = PhysicsComponent::DECOR;
    cpPos.pos += diff;
    context->data->getCol()->agents[eid].bounds.origin += diff;
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
        collisionData->agents[eid].bounds.origin += diff;
        
        auto& cpPhy2 =  ecs::get<cp::Physics>(tid);
        cpPhy2.fPush().curSpeed = 0;
        cpPhy2.fPush().accSpeed = 200;
        cpPhy2.fPush().decSpeed = 250;
        cpPhy2.fPush().maxSpeed = 45;
        cpPhy2.fPush().direction = -diff.getNormalized();
        cpPhy2.fPush().active = true;
        
        //TODO
        //auto& agent2 = collisionData->agents[tid];
        //agent2.velocity = -diff;
        //ecs::get<cp::Physics>(tid).velocity = agent2.velocity;
        
        /*if (!collisionData->checkCollisionRect(agent2.bounds - diff, agent2.category))
        {
            ecs::get<cp::Position>(tid).pos -= diff;
            agent2.bounds.origin -= diff;
            agent2.lastBounds = agent2.bounds;
        }*/
    }
}

void CollisionSystem::tick(double dt)
{
    this->collisionData->process();
}