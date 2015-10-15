#include "MissionMediator.h"
#include "GameCtrl.h"

void MissionMediator::onAddView(MissionScene &scene)
{
    auto floorData = GameCtrl::instance()->getData().curFloor();
    auto playerData = GameCtrl::instance()->getData().curPlayer();
    
    scene.setBgColor(floorData->getBgColor());
    
    floorSystemCtrl.load(scene.getCam(), scene.getFrame(), playerData, floorData);
    floorSystemCtrl.start();
    
    //this->gView->interface->getHealthBar()->initProperties(csHealth.maxHp,
    //                                                       csHealth.hp);
    
    scene.interface->registerPlayer(playerData->ctrlIndex, [playerData](KeyCode code) {
        return playerData->KeyCode2KeyType(code);
    });
    
    //view events
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
            this->floorSystemCtrl.getCtrlSystem()->setStickDirection(pos);
    }));
    
    this->eventRegs.push_back(scene.interface->getStick()->onRelease.registerObserver(
        [this](){
            this->floorSystemCtrl.getCtrlSystem()->releaseStick();
    }));
    
    this->eventRegs.push_back(scene.interface->onKeyPressAction.registerObserver(
        [this](unsigned playerIndex, int flag){
            this->floorSystemCtrl.getCtrlSystem()->setKeyPressAction(flag);
    }));
    
    this->eventRegs.push_back(scene.interface->onKeyReleaseAction.registerObserver(
        [this](unsigned playerIndex, int flag){
            this->floorSystemCtrl.getCtrlSystem()->setKeyReleaseAction(flag);
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
                GameCtrl::instance()->goToCamp("mission");
                break;
            case GateMap::CmdType::EXIT_MAP:
                GameCtrl::instance()->goToCamp("mission");
                break;
            default:
                break;
        }
    }));
}

void MissionMediator::onRemoveView(MissionScene &scene)
{
    floorSystemCtrl.clear();
}

void MissionMediator::onTick(double dt)
{
    floorSystemCtrl.tick(dt);
}

void MissionMediator::onAnimate(double dt, double tickPercent)
{
    floorSystemCtrl.animate(dt, tickPercent);
}