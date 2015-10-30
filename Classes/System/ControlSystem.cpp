#include "GameCtrl.h"
#include "ControlSystem.h"
#include "Components.h"
#include "SysHelper.h"
#include "Defines.h"

ControlSystem::ControlSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {
}

void ControlSystem::tick(double dt)
{
    //selection control
    if (posSelection != nullptr)
    {
        for(auto eid : ecs.join<cp::Interact, cp::Position, cp::Collision>())
        {
            auto eRect = lib::inflateRect(SysHelper::getBounds(eid), def::touchTreshold);
            if (eRect.containsPoint(posSelection.Value))
            {
                ecs::get<cp::Interact>(eid).triggerActivation = true;
                break;
            }
        }
    }
    
    //moving control
    for(auto eid : ecs.join<cp::Control, cp::Input, cp::Control>())
    {
        unsigned ctrlIndex = ecs::get<cp::Control>(eid);
        if (std::find(indexList.begin(), indexList.end(), ctrlIndex) == indexList.end())
            continue;
        
        auto& cpInput = ecs::get<cp::Input>(eid);
         
        if (joyPos[ctrlIndex] != nullptr)
        {
            cpInput.setDirection(joyPos[ctrlIndex].Value);
        }
        else
        {
            cpInput.setDirection(this->curDirPressed[ctrlIndex] |
                (this->curDirReleased[ctrlIndex] & ~this->preDirPressed[ctrlIndex]));
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
                    if (ecs::has<cp::Collision>(tid))
                    {
                        auto& cpRender = ecs::get<cp::Render>(tid);
                        auto& cpCol = ecs::get<cp::Collision>(tid);
                        
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
            
        if (this->actionSelection != nullptr)
        {
            cpInput.actionMode = this->actionSelection.Value;
        }
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

void ControlSystem::init(std::list<unsigned> indexes)
{
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

void ControlSystem::setStickDirection(unsigned index, const lib::Nullable<cc::Vec2>& dir)
{
    this->joyPos[index] = dir;
}