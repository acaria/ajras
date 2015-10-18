#include "MissionMediator.h"
#include "GameCtrl.h"

void MissionMediator::onAddView(MissionScene &scene)
{
    auto floorData = GameCtrl::instance()->getData().curFloor();
    auto playerData = GameCtrl::instance()->getData().curPlayer();
    
    scene.setBgColor(floorData->getBgColor());
    
    floorSystemCtrl.load(scene.getCam(), scene.getFrame(), playerData, floorData);
    floorSystemCtrl.start();
    
    scene.interface->registerPlayer(playerData->ctrlIndex, [playerData](KeyCode code) {
        return playerData->KeyCode2KeyType(code);
    });
    
    scene.interface->getInventoryPanel()->registerPlayer(playerData->entityFocus,
                                                         playerData->inventory);
    
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
            this->floorSystemCtrl.getCtrlSystem()->setStickDirection(nullptr);
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
    
    this->eventRegs.push_back(scene.getCam()->onTouch.registerObserver([this, floorData](cc::Point pos){
        auto localPos = pos - floorData->getCurrentRoom()->getBounds().origin;
        this->floorSystemCtrl.getCtrlSystem()->setSelectionPos(localPos);
    }));
    
    this->eventRegs.push_back(scene.getCam()->onSwipe.registerObserver([this, floorData](
            cc::Point pos1, cc::Point pos2){
        auto localPos1 = pos1 - floorData->getCurrentRoom()->getBounds().origin;
        auto localPos2 = pos2 - floorData->getCurrentRoom()->getBounds().origin;
        Log("swipe=%f,%f -> %f,%f", localPos1.x, localPos1.y, localPos2.x, localPos2.y);
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
    
    this->eventRegs.push_back(floorSystemCtrl.onGearChanged.registerObserver(
            [this, playerData, &scene](unsigned eid, const cp::GearComponent& gear) {
        if (playerData->entityFocus == eid)
        {
            scene.interface->getInventoryPanel()->updatePlayer(eid, gear);
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