#include "GameCtrl.h"
#include "CtrlCampSystem.h"
#include "Components.h"
#include "InputComponent.h"
#include "AIComponent.h"
#include "CampData.h"
#include "CampScene.h"
#include "CampInterfaceLayer.h"
#include "SysHelper.h"
#include "Defines.h"

using namespace std::placeholders;
using KeyCode = cocos2d::EventKeyboard::KeyCode;

CtrlCampSystem::CtrlCampSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {
}

void CtrlCampSystem::tick(double dt)
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
            auto posRatio = this->view->interface->setJoystick(joyPos[player->ctrlIndex]);
            cpInput.setDirection(posRatio);
        }
        else
        {
            this->view->interface->clearJoystick();
            cpInput.setDirection(this->curDirPressed[player->ctrlIndex] |
                                 (this->curDirReleased[player->ctrlIndex] & ~this->preDirPressed[player->ctrlIndex]));
        }
        
        //clear inputs
        this->clearReleased(player->ctrlIndex);
    }
}

void CtrlCampSystem::initControl(unsigned int index)
{
    curDirPressed[index] = Dir::None;
    curDirReleased[index] = Dir::None;
    preDirPressed[index] = Dir::None;
}

void CtrlCampSystem::clearReleased(unsigned index)
{
    this->preDirPressed[index] = this->curDirPressed[index];
    this->curDirReleased[index] = Dir::None;
}

void CtrlCampSystem::init(CampScene *gview, CampData* data)
{
    this->pList.push_back(GameCtrl::instance()->getData().curPlayer());
    
    this->data = data;
    this->view = gview;
    
    //reset ctrl
    for(auto player : pList)
    {
        this->initControl(player->ctrlIndex);
    }
}

cc::Rect CtrlCampSystem::computeRect(cc::Point p1, cc::Point p2)
{
    cc::Rect result;
    result.origin.x = (p1.x < p2.x) ? p1.x : p2.x;
    result.size.width = (p1.x < p2.x) ? p2.x - p1.x : p1.x - p2.x;
    result.origin.y = (p1.y < p2.y) ? p1.y : p2.y;
    result.size.height = (p1.y < p2.y) ? p2.y - p1.y : p1.y - p2.y;
    return result;
}