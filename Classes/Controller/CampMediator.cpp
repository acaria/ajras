#include "CampMediator.h"
#include "GameCtrl.h"

void CampMediator::onAddView(CampScene &scene)
{
    auto campData = GameCtrl::instance()->getData().curCamp();
    auto playerData = GameCtrl::instance()->getData().curPlayer();
    
    scene.setBgColor(cc::Color3B::BLACK);
    scene.getCam()->setFrameBounds(campData->getBounds());
    
    campSystemCtrl.load(scene.getCam(), scene.getFrame(), playerData, campData);
    campSystemCtrl.start();
    
    scene.interface->registerPlayer(playerData->ctrlIndex, [playerData](KeyCode code) {
        return playerData->KeyCode2KeyType(code);
    });
    
    //interface events
    this->eventRegs.push_back(scene.interface->getStick()->onTrigger.registerObserver(
        [this](cc::Vec2 pos){
            this->campSystemCtrl.getCtrlSystem()->setStickDirection(pos);
    }));
    
    this->eventRegs.push_back(scene.interface->getStick()->onRelease.registerObserver(
        [this](){
            this->campSystemCtrl.getCtrlSystem()->releaseStick();
    }));
    
    this->eventRegs.push_back(scene.interface->onKeyPressAction.registerObserver(
        [this](unsigned playerIndex, int flag){
            this->campSystemCtrl.getCtrlSystem()->setKeyPressAction(flag);
    }));
    
    this->eventRegs.push_back(scene.interface->onKeyReleaseAction.registerObserver(
        [this](unsigned playerIndex, int flag){
            this->campSystemCtrl.getCtrlSystem()->setKeyReleaseAction(flag);
    }));
    
    //system events
    this->eventRegs.push_back(campSystemCtrl.onHealthChanged.registerObserver(
            [this, &scene](unsigned int eid, int health) {
    }));
    
    this->eventRegs.push_back(campSystemCtrl.onWarpTriggered.registerObserver(
            [this](unsigned eid, WarpMap warp) {
        if (warp.keyCmd == "home")
        {
            GameCtrl::instance()->goToMainMenu();
        }
        else if (warp.keyCmd == "mission")
        {
            GameCtrl::instance()->goToMission();
        }
        else
        {
            Log("error key cmd not recognised");
        }
    }));
}


void CampMediator::onRemoveView(CampScene &scene)
{
    scene.getEventDispatcher()->removeAllEventListeners();
}

void CampMediator::onTick(double dt)
{
    campSystemCtrl.tick(dt);
}

void CampMediator::onAnimate(double dt, double tickPercent)
{
    campSystemCtrl.animate(dt, tickPercent);
}