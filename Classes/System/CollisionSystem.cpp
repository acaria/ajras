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
        for(auto eid : context->ecs->join<cp::Position, cp::Physics, cp::Input>())
        {
            if (ecs::get<cp::Input>(eid).enabled)
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
        
        this->sysRegs.push_back(this->collisionData->onFakeNodeCollision.registerObserver(
                [this](unsigned eid,cc::Vec2 diff) {
            this->onFakeNodeCollision(eid, diff);
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
    
    this->eventRegs.push_back(this->dispatcher->onFakeAgentNodeAdded.registerObserver(
            [this](unsigned group, cc::Node* node, const cc::Rect& r){
        if (context->ecs->getID() != group)
            return;
        collisionData->fakeNodeAgents[node] = r;
    }));
    
    this->eventRegs.push_back(this->dispatcher->onFakeAgentNodeRemoved.registerObserver(
            [this](unsigned group, cc::Node* node){
        collisionData->fakeNodeAgents.erase(node);
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
    if (ecs::has<cp::Input>(eid) && !ecs::get<cp::Input>(eid).withCollision) //inhibit
        return;
    auto& cpPos = ecs::get<cp::Position>(eid);
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    
    cpPhy.collisionState = PhysicsComponent::DECOR;
    cpPos.pos += diff;
    context->data->getCol()->agents[eid].bounds.origin += diff;
}

void CollisionSystem::onFakeNodeCollision(unsigned eid, cc::Vec2 diff)
{
    if (ecs::has<cp::Input>(eid) && !ecs::get<cp::Input>(eid).withCollision) //inhibit
        return;
    Log("%u=(%f,%f)", eid, diff.x, diff.y);
    ecs::get<cp::Position>(eid).pos -= diff;
    collisionData->agents[eid].bounds.origin -= diff;
}

void CollisionSystem::onAgentCollision(unsigned eid, unsigned tid, cc::Vec2 diff)
{
    if (ecs::has<cp::Input>(eid) && !ecs::get<cp::Input>(eid).withCollision) //inhibit
        return;
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
        
        cc::Vec2 unit = diff.getNormalized();
        
        cc::Vec2 p1 = -cpPhy.getResultForce().project(diff);
        cc::Vec2 res1 = {MAX(0, p1.x * unit.x), MAX(0, p1.y * unit.y)};
        float l1 = res1.getLength();
        
        
        cc::Vec2 p2 = cpPhy2.getResultForce().project(diff);
        cc::Vec2 res2 = {MAX(0, p2.x * unit.x), MAX(0, p2.y * unit.y)};
        float l2 = res2.getLength();
        float fAmount1 = l1 * MIN(1.0,  cpPhy.weight / cpPhy2.weight) * cpPhy.strength;
        float fAmount2 = l2 * MIN(1.0, cpPhy2.weight / cpPhy.weight) * cpPhy2.strength;
        
        if (fabs(fAmount1 - fAmount2) < 0.1)//eid == tid
        {
            //Log("%u:%f == %u:%f", eid, fAmount1, tid, fAmount2);
            cpPhy.collisionState = PhysicsComponent::OBJECT;
            cpPos.pos += diff;
            collisionData->agents[eid].bounds.origin += diff;
            cpPhy2.collisionState = PhysicsComponent::OBJECT;
            cpPos2.pos -= diff;
            collisionData->agents[tid].bounds.origin -= diff;
            
            cpPhy.fInput().curSpeed = 0;
            cpPhy.fImpact().accSpeed = 600;
            cpPhy.fImpact().decSpeed = 250;
            cpPhy.fImpact().maxSpeed = fAmount1;
            cpPhy.fImpact().direction = diff.getNormalized();
            cpPhy.fImpact().active = true;
            cpPhy.fImpact().duration = 0.15;
            
            cpPhy2.fInput().curSpeed = 0;
            cpPhy2.fImpact().accSpeed = 600;
            cpPhy2.fImpact().decSpeed = 250;
            cpPhy2.fImpact().maxSpeed = fAmount2;
            cpPhy2.fImpact().direction = -diff.getNormalized();
            cpPhy2.fImpact().active = true;
            cpPhy2.fImpact().duration = 0.15;
        }
        else if (fAmount1 > fAmount2) //eid -> tid
        {
            //Log("%u:%f => %u:%f", eid, fAmount1, tid, fAmount2);
            cpPhy.collisionState = PhysicsComponent::OBJECT;
            cpPos.pos += diff;
            collisionData->agents[eid].bounds.origin += diff;
            
            cpPhy2.fInput().curSpeed = 0;
            cpPhy2.fPush().accSpeed = 200;
            cpPhy2.fPush().decSpeed = 250;
            cpPhy2.fPush().maxSpeed = fAmount1 - fAmount2;
            cpPhy2.fPush().direction = -diff.getNormalized();
            cpPhy2.fPush().active = true;
        }
        else //tid -> eid
        {
            //Log("%u:%f => %u:%f", tid, fAmount2, eid, fAmount1);
            cpPhy2.collisionState = PhysicsComponent::OBJECT;
            cpPos2.pos -= diff;
            collisionData->agents[tid].bounds.origin -= diff;
            
            cpPhy.fInput().curSpeed = 0;
            cpPhy.fPush().accSpeed = 200;
            cpPhy.fPush().decSpeed = 250;
            cpPhy.fPush().maxSpeed = fAmount2 - fAmount1;
            cpPhy.fPush().direction = diff.getNormalized();
            cpPhy.fPush().active = true;
        }
    }
}

void CollisionSystem::tick(double dt)
{
    this->collisionData->process();
}