#include "CampMediator.h"
#include "GameCtrl.h"

void CampMediator::onAddView(CampScene &scene)
{
    auto campData = GameCtrl::instance()->getData().curCamp();
    auto playerData = GameCtrl::instance()->getData().getPlayerData();
    
    scene.setBgColor(cc::Color3B::BLACK);
    scene.getCam()->setFrameBounds(campData->getBounds());
    
    systemCtrl.load(scene.getCam(), scene.getFrame(), playerData, campData);
    systemCtrl.start();
    
    this->registerDispatcher(scene);
}

void CampMediator::registerDispatcher(CampScene &scene)
{
    auto playerData = GameCtrl::instance()->getData().getPlayerData();
    auto campData = GameCtrl::instance()->getData().curCamp();
    
    this->systemRegs.clear();
    auto& dispatcher = this->systemCtrl.getDispatcher();
    
    //interface events
    this->systemRegs.push_back(scene.interface->getStick()->onTrigger.registerObserver(
            [&dispatcher](cc::Vec2 pos){
        dispatcher.onStickDirection(def::CTRL1, pos);
    }));
    
    this->systemRegs.push_back(scene.interface->getStick()->onRelease.registerObserver(
            [&dispatcher](){
        dispatcher.onStickDirection(def::CTRL1, nullptr);
    }));
    
    this->systemRegs.push_back(scene.interface->onKeyPressAction.registerObserver(
            [playerData, &dispatcher](KeyCode code){
        auto pair = playerData->KeyCode2KeyType(code);
        if (pair.first != 0 || pair.second == CtrlKeyType::none)
        {
            Dir dir = Dir::fromCtrlKeyType(pair.second);
            
            if (dir != Dir::None) //key direction
                dispatcher.onKeyPressDirection(pair.first, dir.getRaw());
        }
    }));
    
    this->systemRegs.push_back(scene.interface->onKeyReleaseAction.registerObserver(
            [playerData, &dispatcher](KeyCode code){
        auto pair = playerData->KeyCode2KeyType(code);
        if (pair.first != 0 || pair.second == CtrlKeyType::none)
        {
            Dir dir = Dir::fromCtrlKeyType(pair.second);
            
            if (dir != Dir::None)
                dispatcher.onKeyReleaseDirection(pair.first, dir.getRaw());
        }
    }));
    
    this->systemRegs.push_back(scene.getCam()->onTouch.registerObserver(
            [this, campData, &dispatcher](cc::Point pos){
        auto localPos = pos - campData->getBounds().origin;
        dispatcher.onSelectionPos(localPos);
    }));
    
    this->systemRegs.push_back(scene.getCam()->onSwipe.registerObserver([this](cc::Point pos1,
                                                                              cc::Point pos2){
        GameCtrl::log("swipe=%f,%f -> %f,%f", pos1.x, pos1.y, pos2.x, pos2.y);
    }));
    
    this->systemRegs.push_back(dispatcher.onWarpTriggered.registerObserver(
            [&playerData](unsigned eid, WarpMap warp) {
        if (eid != playerData->getEntityFocusID())
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