#include "GameCtrl.h"
#include "ControlSystem.h"
#include "Components.h"
#include "Defines.h"

ControlSystem::ControlSystem(std::list<unsigned> indexes) {
    this->indexList = indexes;
    
    for(auto index : this->indexList)
    {
        curDirPressed[index] = Dir::None;
        curDirReleased[index] = Dir::None;
        preDirPressed[index] = Dir::None;
        joyPos[index] = nullptr;
    }
    posSelection = nullptr;
    actionSelection = nullptr;
}

void ControlSystem::init()
{
    using namespace std::placeholders;
    
    this->eventRegs.push_back(dispatcher->onStickDirection.registerObserver(
        std::bind(&ControlSystem::setStickDirection, this, _1, _2)));
    this->eventRegs.push_back(dispatcher->onKeyPressAction.registerObserver(
        std::bind(&ControlSystem::setKeyPressAction, this, _1, _2)));
    this->eventRegs.push_back(dispatcher->onKeyReleaseAction.registerObserver(
        std::bind(&ControlSystem::setKeyReleaseAction, this, _1, _2)));
    this->eventRegs.push_back(dispatcher->onSelectionAction.registerObserver(
        std::bind(&ControlSystem::setSelectionAction, this, _1)));
    this->eventRegs.push_back(dispatcher->onSelectionPos.registerObserver(
        std::bind(&ControlSystem::setSelectionPos, this, _1)));
}

void ControlSystem::tick(double dt)
{
    //selection control
    if (posSelection != nullptr)
    {
        bool handled = false;
        for(auto eid : context->ecs->join<cp::Interact, cp::Position, cp::Physics>())
        {
            auto eRect = lib::inflateRect(SysHelper::getBounds(eid), def::touchTreshold);
            if (eRect.containsPoint(posSelection.Value))
            {
                handled = true;
                ecs::get<cp::Interact>(eid).triggerActivation = true;
                break;
            }
        }
        
        if (!handled)
        {
            for(auto eid : context->ecs->join<cp::Control, cp::Input>())
            {
                handled = true;
                ecs::get<cp::Input>(eid).goTo = posSelection;
                break;
            }
        }
    }
    
    //moving control
    for(auto eid : context->ecs->join<cp::Control, cp::Input, cp::Control>())
    {
        unsigned ctrlIndex = ecs::get<cp::Control>(eid);
        if (std::find(indexList.begin(), indexList.end(), ctrlIndex) == indexList.end())
            continue;
        
        auto& cpInput = ecs::get<cp::Input>(eid);
         
        if (joyPos[ctrlIndex] != nullptr)
        {
            cpInput.direction = joyPos[ctrlIndex].Value;
        }
        else
        {
            cpInput.direction = Dir(this->curDirPressed[ctrlIndex] |
                (this->curDirReleased[ctrlIndex] & ~this->preDirPressed[ctrlIndex])).toVec();
        }
        
        //selection
        /*if (this->entitySelection[player->ctrlIndex] != 0)
        {
            auto tid = this->entitySelection[player->ctrlIndex];
            switch(this->view->interface->getAction())
            {
                case ActionMode::none:
                    Log("Action mode is missing");
                    break;
                case ActionMode::attack:
                    if (ecs::has<cp::Physics>(tid))
                    {
                        auto& cpRender = ecs::get<cp::Render>(tid);
                        auto& cpCol = ecs::get<cp::Physics>(tid);
                        
                        auto pos = cc::Point(
                            cpCol.rect.getMinX() + cpCol.rect.size.width / 2,
                            cpCol.rect.getMinY() + cpCol.rect.size.height / 2);
                        
                        this->view->interface->setTargetID(
                            tid,
                            ecs::has<cp::Control>(tid),
                            cpRender.sprite, pos);
                        ecs.add<cp::Target>(eid) = tid;
                    }
                    break;
                case ActionMode::explore:
                    if (ecs::has<cp::Interact>(tid))
                    {
                        ecs::get<cp::Interact>(tid).triggerActivation = true;
                    }
                    break;
                case ActionMode::inventorize:
                    break;
            }
        }*/
    }
    
    this->clearReleased();
}

void ControlSystem::clearReleased()
{
    for(unsigned index : this->indexList)
    {
        this->preDirPressed[index] = this->curDirPressed[index];
        this->curDirReleased[index] = Dir::None;
    }
    
    this->posSelection = nullptr;
    this->actionSelection = nullptr;
}

void ControlSystem::setSelectionAction(ActionMode mode)
{
    this->actionSelection = mode;
}

void ControlSystem::setSelectionPos(cc::Point p)
{
    this->posSelection = p;
}

void ControlSystem::setKeyPressAction(unsigned index, int flag)
{
    this->curDirPressed[index] |= flag;
    this->curDirReleased[index] &= ~flag;
}

void ControlSystem::setKeyReleaseAction(unsigned index, int flag)
{
    this->curDirPressed[index] &= ~flag;
    this->curDirReleased[index] |= flag;
}

void ControlSystem::setStickDirection(unsigned index,
                                      const lib::Nullable<cc::Vec2>& dir)
{
    this->joyPos[index] = dir;
}