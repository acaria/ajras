#include "CampMediator.h"
#include "GameCtrl.h"

void CampMediator::onAddView(CampScene &scene)
{
    auto campData = GameCtrl::instance()->getData().curCamp();
    auto playerData = GameCtrl::instance()->getData().curPlayer();
    
    scene.setBgColor(cc::Color3B::BLACK);
    scene.getCam()->setFrameBounds(campData->getBounds());
    
    systemCtrl.load(scene.getCam(), scene.getFrame(), playerData, campData);
    systemCtrl.start();
    
    scene.interface->registerIndex(playerData->ctrlIndex, [playerData](KeyCode code) {
        return playerData->KeyCode2KeyType(code);
    });
    
    this->registerDispatcher(scene);
}

void CampMediator::registerDispatcher(CampScene &scene)
{
    auto playerData = GameCtrl::instance()->getData().curPlayer();
    auto campData = GameCtrl::instance()->getData().curCamp();
    
    this->systemRegs.clear();
    auto& dispatcher = this->systemCtrl.getDispatcher();
    
    //interface events
    unsigned pIndex = playerData->ctrlIndex;
    this->systemRegs.push_back(scene.interface->getStick()->onTrigger.registerObserver(
            [this, pIndex, &dispatcher](cc::Vec2 pos){
        dispatcher.onStickDirection(pIndex, pos);
    }));
    
    this->systemRegs.push_back(scene.interface->getStick()->onRelease.registerObserver(
            [this, pIndex, &dispatcher](){
        dispatcher.onStickDirection(pIndex, nullptr);
    }));
    
    this->systemRegs.push_back(scene.interface->onKeyPressAction.registerObserver(
            [this, &dispatcher](unsigned index, int flag){
        dispatcher.onKeyPressAction(index, flag);
    }));
    
    this->systemRegs.push_back(scene.interface->onKeyReleaseAction.registerObserver(
            [this, &dispatcher](unsigned index, int flag){
        dispatcher.onKeyReleaseAction(index, flag);
    }));
    
    this->systemRegs.push_back(scene.getCam()->onTouch.registerObserver(
            [this, campData, &dispatcher](cc::Point pos){
        auto localPos = pos - campData->getBounds().origin;
        dispatcher.onSelectionPos(localPos);
    }));
    
    this->systemRegs.push_back(scene.getCam()->onSwipe.registerObserver([this](cc::Point pos1,
                                                                              cc::Point pos2){
        Log("swipe=%f,%f -> %f,%f", pos1.x, pos1.y, pos2.x, pos2.y);
    }));
    
    this->systemRegs.push_back(dispatcher.onWarpTriggered.registerObserver(
            [this](unsigned eid, WarpMap warp) {
        if (eid != GameCtrl::instance()->getData().curPlayer()->entityFocus)
            return;
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
    systemRegs.clear();
    systemCtrl.clear();
}

void CampMediator::onTick(double dt)
{
    systemCtrl.tick(dt);
}

void CampMediator::onAnimate(double dt, double tickPercent)
{
    systemCtrl.animate(dt, tickPercent);
}