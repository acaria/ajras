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
    
    this->eventRegs.push_back(this->dispatcher->onEntityMoved.registerObserver(
            [this](unsigned group, unsigned eid) {
        collisionData->agents.erase(eid);
    }));
    
    this->eventRegs.push_back(this->dispatcher->onFakeAgentNodeAdded.registerObserver(
            [this](unsigned group, cc::Node* node, const cc::Rect& r){
        if (context->ecs->getID() != group)
            return;
        collisionData->fakeNodeAgents[node] = r;
    }));
    
    this->eventRegs.push_back(this->dispatcher->onFakeAgentRectAdded.registerObserver(
            [this](unsigned group, unsigned eid, const cc::Rect& r){
        if (context->ecs->getID() != group)
            return;
        collisionData->fakeRectAgents[eid] = r;
    }));
    
    this->eventRegs.push_back(this->dispatcher->onFakeAgentNodeRemoved.registerObserver(
            [this](unsigned group, cc::Node* node){
        collisionData->fakeNodeAgents.erase(node);
    }));
    
    this->eventRegs.push_back(this->dispatcher->onFakeAgentRectRemoved.registerObserver(
            [this](unsigned group, unsigned eid){
        collisionData->fakeRectAgents.erase(eid);
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

void CollisionSystem::onFakeNodeCollision(unsigned eid, cc::Vec2 diff)
{
    ecs::get<cp::Position>(eid).pos -= diff;
    collisionData->agents[eid].bounds.origin -= diff;
}

void CollisionSystem::agentCollectResolution(unsigned eid, unsigned tid, cc::Vec2 diff)
{
    if (!ecs::has<cp::Collectible>(tid))
        return;
    auto collectible = ModelProvider::instance()->collectible.get(ecs::get<cp::Collectible>(tid));
    
    bool triggerCollect = false;
    if (collectible->currency)
    {
        if (SysHelper::getCurrencyAvailable(eid, collectible))
        {
            dispatcher->onGearCurrencyChanged(eid, collectible->currencyValue);
            triggerCollect = true;
        }
    }
    else
    {
        SlotData* slot = SysHelper::getAvailableSlot(eid, collectible);
        if (slot != nullptr)
        {
            dispatcher->onGearSlotChanged(eid, slot);
            triggerCollect = true;
        }
    }
    
    if (triggerCollect)
    {
        ecs::get<cp::Render>(tid).sprite->removeFromParent();
        cp::entity::remove(tid, context->ecs->getID());
    }
}

void CollisionSystem::agentObstacleResolution(unsigned eid, unsigned tid, cc::Vec2 diff)
{
    auto& cpPos = ecs::get<cp::Position>(eid);
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    auto& cpPos2 = ecs::get<cp::Position>(tid);
    auto& cpPhy2 =  ecs::get<cp::Physics>(tid);
    
    cc::Vec2 unit = diff.getNormalized();
    
    if (cpPhy.weight == 0)
    {
        cpPhy2.collisionState = PhysicsComponent::OBJECT;
        cpPos2.pos -= diff;
        collisionData->agents[tid].bounds.origin -= diff;
        return;
    }
    
    if (cpPhy2.weight == 0)
    {
        cpPhy.collisionState = PhysicsComponent::OBJECT;
        cpPos.pos += diff;
        collisionData->agents[eid].bounds.origin += diff;
        return;
    }
    
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
        Log("%u:%f == %u:%f", eid, fAmount1, tid, fAmount2);
        cpPhy.collisionState = PhysicsComponent::OBJECT;
        cpPos.pos += diff;
        collisionData->agents[eid].bounds.origin += diff;
        cpPhy2.collisionState = PhysicsComponent::OBJECT;
        cpPos2.pos -= diff;
        collisionData->agents[tid].bounds.origin -= diff;
        
        cpPhy.resetInput();
        cpPhy.setImpact(fAmount1, 3, unit, 0.15);
        
        cpPhy2.resetInput();
        cpPhy.setImpact(fAmount2, 3, -unit, 0.15);
    }
    else if (fAmount1 > fAmount2) //eid -> tid
    {
        Log("%u:%f => %u:%f", eid, fAmount1, tid, fAmount2);
        cpPhy.collisionState = PhysicsComponent::OBJECT;
        cpPos.pos += diff;
        collisionData->agents[eid].bounds.origin += diff;
        
        cpPhy2.resetInput();
        cpPhy2.setPush(fAmount1 - fAmount2, -unit);
    }
    else //tid -> eid
    {
        Log("%u:%f => %u:%f", tid, fAmount2, eid, fAmount1);
        cpPhy2.collisionState = PhysicsComponent::OBJECT;
        cpPos2.pos -= diff;
        collisionData->agents[tid].bounds.origin -= diff;
        
        cpPhy.resetInput();
        cpPhy.setPush(fAmount2 - fAmount1, unit);
    }
}

void CollisionSystem::agentTeamResolution(unsigned eid, unsigned tid, cc::Vec2 diff)
{
    auto b1 = SysHelper::getBounds(eid);
    auto b2 = SysHelper::getBounds(tid);
    cc::Vec2 unit = cc::Vec2(b2.getMidX() - b1.getMidX(), b2.getMidY() - b1.getMidY()).getNormalized();
    if (ecs::get<cp::Team>(eid).position < ecs::get<cp::Team>(tid).position)
    {
        ecs::get<cp::Physics>(eid).resetInput();
        ecs::get<cp::Physics>(tid).setImpact(30, 8, unit, 0.15);
    }
    else
    {
        ecs::get<cp::Physics>(tid).resetInput();
        ecs::get<cp::Physics>(eid).setImpact(30, 8, -unit, 0.15);
    }
}

void CollisionSystem::onAgentCollision(unsigned eid, unsigned tid, cc::Vec2 diff)
{
    if (collisionData->agents[tid].category == def::collision::Cat::collectible)
    {
        this->agentCollectResolution(eid, tid, diff);
    }
    else //obstacle
    {
        if (ecs::has<cp::Team>(eid) && ecs::has<cp::Team>(tid) &&
            ecs::get<cp::Team>(eid).index == ecs::get<cp::Team>(tid).index)
        {
            this->agentTeamResolution(eid, tid, diff);
        }
        else
        {
            this->agentObstacleResolution(eid, tid, diff);
        }
    }
}

void CollisionSystem::tick(double dt)
{
    this->collisionData->process();
}