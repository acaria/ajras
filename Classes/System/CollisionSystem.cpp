#include "CollisionSystem.h"
#include "ModelProvider.h"
#include "IMapData.h"
#include "SysHelper.h"

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

#include "GameCtrl.h"
void CollisionSystem::onAgentCollision(unsigned eid, unsigned tid, cc::Vec2 diff)
{
    Log("col tick=%lu, %u => %u, %f - %f", GameCtrl::instance()->tick.getTicks(), eid, tid, diff.x, diff.y);
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
                dispatcher->onGearChanged(eid, cpGear);
            }
        }
    }
    else //obstacle
    {
        auto& cpPos2 = ecs::get<cp::Position>(tid);
        auto& cpPhy2 =  ecs::get<cp::Physics>(tid);
        
        auto r = SysHelper::getBounds(cpPos, cpPhy);
        auto r2 = SysHelper::getBounds(cpPos2, cpPhy2);
        
        
        cc::Vec2 v1 = {r2.getMidX() - r.getMidX(), r2.getMidY() - r.getMidY()};
        auto l1 = cpPhy.getResultForce().project(v1).getLengthSq();
        
        cc::Vec2 v2 = {r.getMidX() - r2.getMidX(), r.getMidY() - r2.getMidY()};
        auto l2 = cpPhy2.getResultForce().project(v2).getLengthSq();
        
        if (false/*fabs(l1 - l2) < 5*/) //no forces ->
        {
            cpPhy.collisionState = PhysicsComponent::OBJECT;
            cpPos.pos += diff;
            collisionData->agents[eid].bounds.origin += diff;
        }
        else
        {
            if (l1 < l2)
            {
                cpPhy2.collisionState = PhysicsComponent::OBJECT;
                cpPos2.pos -= diff;
                collisionData->agents[tid].bounds.origin -= diff;
                
                //cpPhy.fPush().curSpeed = 0;
                cpPhy.fPush().accSpeed = 200;
                cpPhy.fPush().decSpeed = 250;
                cpPhy.fPush().maxSpeed = 45;
                cpPhy.fPush().direction = diff.getNormalized();
                cpPhy.fPush().active = true;
            }
            else
            {
                cpPhy.collisionState = PhysicsComponent::OBJECT;
                cpPos.pos += diff;
                collisionData->agents[eid].bounds.origin += diff;
                
                //cpPhy2.fPush().curSpeed = 0;
                cpPhy2.fPush().accSpeed = 200;
                cpPhy2.fPush().decSpeed = 250;
                cpPhy2.fPush().maxSpeed = 45;
                cpPhy2.fPush().direction = -diff.getNormalized();
                cpPhy2.fPush().active = true;
            }
        }
        
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