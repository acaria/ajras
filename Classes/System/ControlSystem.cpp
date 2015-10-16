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
    for(auto eid : ecs.join<cp::Control, cp::Input>())
    {
        auto feid = player->entityFocus;
        if (feid == 0)
            continue;
        
        //direction
        if (ecs::get<cp::Control>(eid) != player->ctrlIndex)
            continue;
        auto& cpInput = ecs::get<cp::Input>(eid);
         
        if (joyPos != nullptr)
        {
            cpInput.setDirection(joyPos.Value);
        }
        else
        {
            cpInput.setDirection(this->curDirPressed |
                (this->curDirReleased & ~this->preDirPressed));
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
    this->preDirPressed = this->curDirPressed;
    this->curDirReleased = Dir::None;
    this->posSelection = nullptr;
    this->actionSelection = nullptr;
}

void ControlSystem::init(PlayerData* player)
{
    this->player = player;
    
    curDirPressed = Dir::None;
    curDirReleased = Dir::None;
    preDirPressed = Dir::None;
    posSelection = nullptr;
    joyPos = nullptr;
}

void ControlSystem::setSelectionAction(ActionMode mode)
{
    this->actionSelection = mode;
}

void ControlSystem::setSelectionPos(cc::Point p)
{
    this->posSelection = p;
}

void ControlSystem::setKeyPressAction(int flag)
{
    this->curDirPressed |= flag;
    this->curDirReleased &= ~flag;
}

void ControlSystem::setKeyReleaseAction(int flag)
{
    this->curDirPressed &= ~flag;
    this->curDirReleased |= flag;
}

void ControlSystem::setStickDirection(const lib::Nullable<cc::Vec2>& dir)
{
    joyPos = dir;
}