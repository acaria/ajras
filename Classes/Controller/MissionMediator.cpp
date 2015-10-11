#include "MissionMediator.h"
#include "MissionScene.h"
#include "GameCtrl.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

void MissionMediator::onAddView(MissionScene &scene)
{
    floorSystemCtrl.load(&scene, GameCtrl::instance()->getData().curFloor());
    floorSystemCtrl.start();
    
    auto player = GameCtrl::instance()->getData().curPlayer();
    
    scene.interface->registerPlayer(player->ctrlIndex, [player](KeyCode code) {
        return player->KeyCode2KeyType(code);
    });
    
    //mediator events
    this->eventRegs.push_back(scene.onEnterAfterTransition.registerObserver([](){
        Log("MissionScene after");
    }));
    
    this->eventRegs.push_back(scene.onMenuCmd.registerObserver(
        [&scene](MissionScene::CmdTag tag) {
            switch(tag)
            {
                case MissionScene::CmdTag::QUIT:
                    GameCtrl::instance()->goToMainMenu();
                    break;
                default:
                    Log("unsupported ItemTag in MissionMediator");
                    break;
            }
    }));
    
    //view events
    this->eventRegs.push_back(scene.interface->getStick()->onTrigger.registerObserver(
        [this](cc::Vec2 pos){
            auto player = GameCtrl::instance()->getData().curPlayer();
            this->floorSystemCtrl.getCtrlSystem()->setStickDirection(player->ctrlIndex, pos);
    }));
    
    this->eventRegs.push_back(scene.interface->getStick()->onRelease.registerObserver(
        [this](){
            auto player = GameCtrl::instance()->getData().curPlayer();
            this->floorSystemCtrl.getCtrlSystem()->releaseStick(player->ctrlIndex);
    }));
    
    this->eventRegs.push_back(scene.interface->onKeyPressAction.registerObserver(
        [this](unsigned playerIndex, int flag){
            this->floorSystemCtrl.getCtrlSystem()->setKeyPressAction(playerIndex, flag);
    }));
    
    this->eventRegs.push_back(scene.interface->onKeyReleaseAction.registerObserver(
        [this](unsigned playerIndex, int flag){
            this->floorSystemCtrl.getCtrlSystem()->setKeyReleaseAction(playerIndex, flag);
    }));
    
    this->eventRegs.push_back(scene.interface->onSetActionMode.registerObserver(
        [this](ActionMode mode){
            this->floorSystemCtrl.getCtrlSystem()->setSelectionAction(mode);
    }));
}

void MissionMediator::onRemoveView(MissionScene &scene)
{
    scene.getEventDispatcher()->removeAllEventListeners();
}

void MissionMediator::onTick(double dt)
{
    floorSystemCtrl.tick(dt);
}

void MissionMediator::onAnimate(double dt, double tickPercent)
{
    floorSystemCtrl.animate(dt, tickPercent);
}