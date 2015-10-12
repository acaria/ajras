#include "MissionMediator.h"
#include "MissionScene.h"
#include "GameCtrl.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

void MissionMediator::onAddView(MissionScene &scene)
{
    auto floorData = GameCtrl::instance()->getData().curFloor();
    scene.setBgColor(floorData->getBgColor());
    
    floorSystemCtrl.load(scene.getCam(), scene.getFrame(), floorData);
    floorSystemCtrl.start();
    
    //this->gView->interface->getHealthBar()->initProperties(csHealth.maxHp,
    //                                                       csHealth.hp);
    
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
    
    //interface events
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
    
    //system events
    this->eventRegs.push_back(floorSystemCtrl.onHealthChanged.registerObserver(
            [this, &scene](unsigned int roomIndex, unsigned int eid, int health) {
        if (health == 0)
        {
            scene.interface->unsetTargetID(eid);
        }
        //if (eid == playerFocus->entityFocus)
        //{
        //    this->gView->interface->getHealthBar()->updateProperties(health);
        //}
    }));
    
    this->eventRegs.push_back(floorSystemCtrl.onGateTriggered.registerObserver(
            [this](unsigned prevRoomIndex, unsigned eid, GateMap  gate) {
        switch(gate.cmd)
        {
            case GateMap::CmdType::ENTER_MAP:
                break;
            case GateMap::CmdType::EXIT_MAP:
                break;
            default:
                break;
        }
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