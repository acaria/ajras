#include "GameCtrl.h"
#include "CtrlMissionSystem.h"
#include "Components.h"
#include "InputComponent.h"
#include "AIComponent.h"
#include "RoomData.h"
#include "MissionScene.h"
#include "MissionInterfaceLayer.h"
#include "SysHelper.h"
#include "Defines.h"

CtrlMissionSystem::CtrlMissionSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {
    this->actionSelection = ActionMode::none;
}

void CtrlMissionSystem::tick(double dt)
{
    for(auto player : pList)
    {
        auto eid = player->entityFocus;
        if (eid == 0)
            continue;
        
        //direction
        if (ecs::get<cp::Control>(eid) != player->ctrlIndex)
            continue;
        auto& cpInput = ecs::get<cp::Input>(eid);
         
        if (lib::hasKey(joyPos, player->ctrlIndex))
        {
            cpInput.setDirection(joyPos[player->ctrlIndex]);
        }
        else
        {
            cpInput.setDirection(this->curDirPressed[player->ctrlIndex] |
                (this->curDirReleased[player->ctrlIndex] & ~this->preDirPressed[player->ctrlIndex]));
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
        this->clearReleased(player->ctrlIndex);
    }
}

void CtrlMissionSystem::initControl(unsigned int index)
{
    curDirPressed[index] = Dir::None;
    curDirReleased[index] = Dir::None;
    preDirPressed[index] = Dir::None;
    entitySelection[index] = 0;
}

void CtrlMissionSystem::clearReleased(unsigned index)
{
    this->preDirPressed[index] = this->curDirPressed[index];
    this->curDirReleased[index] = Dir::None;
    this->entitySelection[index] = 0;
    this->actionSelection = ActionMode::none;
}

void CtrlMissionSystem::changeRoom(RoomData* data)
{
    this->data = data;
}

void CtrlMissionSystem::init(RoomData* data)
{
    //todo?: multiplayer management???
    this->pList.push_back(GameCtrl::instance()->getData().curPlayer());
    
    this->data = data;
    
    //reset ctrl
    for(auto player : pList)
    {
        this->initControl(player->ctrlIndex);
    }
}

void CtrlMissionSystem::setSelectionAction(ActionMode mode)
{
    this->actionSelection = mode;
}

void CtrlMissionSystem::setKeyPressAction(unsigned pindex, int flag)
{
    this->curDirPressed[pindex] |= flag;
    this->curDirReleased[pindex] &= ~flag;
}

void CtrlMissionSystem::setKeyReleaseAction(unsigned pindex, int flag)
{
    this->curDirPressed[pindex] &= ~flag;
    this->curDirReleased[pindex] |= flag;
}

void CtrlMissionSystem::setTouchFrameBegan(unsigned pindex, cc::Point camPos, cc::Point touchPos)
{
    cc::Vec2 minColSize = {20,20};
    cc::Vec2 roomPos = {
        data->getBounds().getMinX() + camPos.x,
        data->getBounds().getMinY() + camPos.y
    };
    
    cc::Vec2 pos = {touchPos.x - roomPos.x, touchPos.y - roomPos.y};
    
    for(auto eid : ecs.join<cp::Position, cp::Collision, cp::Render>())
    {
        auto& cpPos = ecs::get<cp::Position>(eid);
        auto& cpCol = ecs::get<cp::Collision>(eid);
        
        cc::Vec2 plus = {
            MAX(0, (minColSize.x - cpCol.rect.size.width) / 2),
            MAX(0, (minColSize.y - cpCol.rect.size.height) / 2)
        };
        
        auto bound = cc::Rect(cpPos.pos.x + cpCol.rect.origin.x - plus.x,
                              cpPos.pos.y + cpCol.rect.origin.y - plus.y,
                              cpCol.rect.size.width + plus.x * 2,
                              cpCol.rect.size.height + plus.y * 2);
        if (bound.containsPoint(pos))
        {
            entitySelection[pindex] = eid;
            break;
        }
    }
}

void CtrlMissionSystem::setStickDirection(unsigned pindex, cc::Vec2 dir)
{
    joyPos[pindex] = dir;
}

void CtrlMissionSystem::releaseStick(unsigned pindex)
{
    joyPos.erase(pindex);
}


/*void CtrlMissionSystem::onMouseDown(cocos2d::Event *event)
{
    cc::Vec2 minColSize = {20,20};
 
    cc::EventMouse* e = (cc::EventMouse*)event;
 
    auto cameraPos = this->view->getCam()->getOrigin();
    cc::Vec2 roomPos = {
        data->getBounds().getMinX() + cameraPos.x,
        data->getBounds().getMinY() + cameraPos.y
    };
    
    cc::Vec2 pos = {e->getCursorX() - roomPos.x - kCanvasMissionRect.origin.x,
        e->getCursorY() - roomPos.y - kCanvasMissionRect.origin.y};
}*/

/*void CtrlMissionSystem::onTouchCanceled(const std::vector<cc::Touch*>& touches, cocos2d::Event* event)
{
    
}

void CtrlMissionSystem::onTouchEnded(const std::vector<cc::Touch*>& touches, cocos2d::Event* event)
{
    for(auto player : pList)
    {
        for(auto touch : touches)
        {
            //handle joystick
            if (lib::hasKey(joyID, player->ctrlIndex) && joyID[player->ctrlIndex] == touch->getID())
            {
                joyID.erase(player->ctrlIndex);
            }
        
            //handle action buttons
            if (this->view->interface->getActionBounds().containsPoint(touch->getStartLocation()))
            {
                auto diff = touch->getLocation() - touch->getStartLocation();
                ActionMode doAction = ActionMode::none;
                if (diff.x < -40)
                    doAction = this->view->interface->getPrevAction();
                else if (diff.x > 40)
                    doAction = this->view->interface->getNextAction();
            
                if (doAction != ActionMode::none)
                {
                    actionSelection = doAction;
                }
            }
            
            //handle camera
            cameraID.erase(touch->getID());
        }
    }
}*/

cc::Rect CtrlMissionSystem::computeRect(cc::Point p1, cc::Point p2)
{
    cc::Rect result;
    result.origin.x = (p1.x < p2.x) ? p1.x : p2.x;
    result.size.width = (p1.x < p2.x) ? p2.x - p1.x : p1.x - p2.x;
    result.origin.y = (p1.y < p2.y) ? p1.y : p2.y;
    result.size.height = (p1.y < p2.y) ? p2.y - p1.y : p1.y - p2.y;
    return result;
}

/*void CtrlMissionSystem::onTouchMoved(const std::vector<cc::Touch*>& touches, cocos2d::Event* event)
{
    std::map<int, cc::Point> movedCameraID;
 
    for(auto player : pList)
    {
        for(auto touch : touches)
        {
            auto touchPos = touch->getLocation();
        
            if (lib::hasKey(joyID, player->ctrlIndex) &&
                joyID[player->ctrlIndex] == touch->getID()) //handle joystick
            {
                joyPos[player->ctrlIndex] = touchPos;
            }
            
            if (lib::hasKey(cameraID, touch->getID()) && cameraID.size() > 1)
            {
                movedCameraID[touch->getID()] = touchPos;
            }
        }
    }
    
    //compute camera handler
    if (movedCameraID.size() > 0)
    {
        auto firstEl = cameraID.begin();
        auto secondEl = ++cameraID.begin();
        auto oldRect = computeRect(firstEl->second, secondEl->second);
        auto pt1 = lib::hasKey(movedCameraID, firstEl->first) ?
            movedCameraID[firstEl->first]:firstEl->second;
        auto pt2 = lib::hasKey(movedCameraID, secondEl->first) ?
            movedCameraID[secondEl->first]:secondEl->second;
        auto newRect = computeRect(pt1, pt2);
        
        if (newRect.size.width > 10 && newRect.size.height > 10)
        {
            auto translateValue = newRect.origin - oldRect.origin;
            auto scaleValue =  (oldRect.size.width + oldRect.size.height) /
                               (newRect.size.width + newRect.size.height);
            this->view->getCam()->addScale(1 - scaleValue);
            this->view->getCam()->translate(translateValue);
        }
        
        for(auto moved : movedCameraID) //update pos
            cameraID[moved.first] = moved.second;
    }
}*/