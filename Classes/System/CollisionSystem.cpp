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
        
        cc::Vec2 unit = {diff.x / fabsf(diff.x), diff.y / fabsf(diff.y)};
        
        cc::Vec2 p1 = -cpPhy.getResultForce().project(diff);
        cc::Vec2 res1 = {MAX(0, p1.x * unit.x), MAX(0, p1.y * unit.y)};
        float l1 = res1.getLength();
        
        
        cc::Vec2 p2 = cpPhy2.getResultForce().project(diff);
        cc::Vec2 res2 = {MAX(0, p2.x * unit.x), MAX(0, p2.y * unit.y)};
        float l2 = res2.getLength();
        
        if ((res2.x + res2.y == 0 && res1.x + res1.y == 0) || fabs(l1 - l2) < 0.1)//eid == tid
        {
            Log("%u == %u", eid, tid);
            cpPhy.fInput().curSpeed = 0;
            cpPhy2.fInput().curSpeed = 0;
            cpPhy.collisionState = PhysicsComponent::OBJECT;
            cpPhy2.collisionState = PhysicsComponent::OBJECT;
            cpPos.pos += diff;
            collisionData->agents[eid].bounds.origin += diff;
        }
        else if (l1 > l2) //eid -> tid
        {
            Log("%u => %u : %f", tid, eid, l1 -l2);
            cpPhy.collisionState = PhysicsComponent::OBJECT;
            cpPos.pos += diff;
            collisionData->agents[eid].bounds.origin += diff;
                
            cpPhy2.fPush().accSpeed = 200;
            cpPhy2.fPush().decSpeed = 250;
            cpPhy2.fPush().maxSpeed = l1 - l2;
            cpPhy2.fPush().direction = -diff.getNormalized();
            cpPhy2.fPush().active = true;
        }
        else //tid -> eid
        {
            Log("%u => %u : %f", tid, eid, l2 -l1);
            cpPhy2.collisionState = PhysicsComponent::OBJECT;
            cpPos2.pos -= diff;
            collisionData->agents[tid].bounds.origin -= diff;
                
            cpPhy.fPush().accSpeed = 200;
            cpPhy.fPush().decSpeed = 250;
            cpPhy.fPush().maxSpeed = l2 - l1;
            cpPhy.fPush().direction = diff.getNormalized();
            cpPhy.fPush().active = true;
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