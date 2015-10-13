#include "GameCtrl.h"
#include "ControlSystem.h"
#include "Components.h"
#include "InputComponent.h"
#include "AIComponent.h"
#include "RoomData.h"
#include "MissionScene.h"
#include "MissionInterface.h"
#include "SysHelper.h"
#include "Defines.h"

ControlSystem::ControlSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {
    this->actionSelection = ActionMode::none;
}

void ControlSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Control, cp::Input>())
    {
        auto feid = player->entityFocus;
        if (feid == 0)
            continue;
        
        //direction
        if (ecs::get<cp::Control>(eid) != player->ctrlIndex)
            continue;
        auto& cpInput = ecs::get<cp::Input>(eid);
         
        if (joyUpdate)
        {
            cpInput.setDirection(joyPos);
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
            
        if (this->actionSelection != ActionMode::none)
        {
            cpInput.actionMode = this->actionSelection;
        }
        
        //clear inputs
        this->clearReleased();
    }
}

void ControlSystem::clearReleased()
{
    this->preDirPressed = this->curDirPressed;
    this->curDirReleased = Dir::None;
    this->entitySelection = 0;
    this->joyUpdate = false;
    this->actionSelection = ActionMode::none;
}

void ControlSystem::init(PlayerData* player)
{
    this->player = player;
    
    curDirPressed = Dir::None;
    curDirReleased = Dir::None;
    preDirPressed = Dir::None;
    entitySelection = 0;
    joyPos = cc::Vec2::ZERO;
}

void ControlSystem::setSelectionAction(ActionMode mode)
{
    this->actionSelection = mode;
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

void ControlSystem::setStickDirection(cc::Vec2 dir)
{
    joyUpdate = true;
    joyPos = dir;
}

void ControlSystem::releaseStick()
{
    joyUpdate = true;
    joyPos = cc::Vec2::ZERO;
}