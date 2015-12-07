#include "MissionMediator.h"
#include "GameCtrl.h"

void MissionMediator::onAddView(MissionScene &scene)
{
    auto floorData = GameCtrl::instance()->getData().curFloor();
    auto playerData = GameCtrl::instance()->getData().curPlayer();
    
    scene.setBgColor(floorData->getBgColor());
    
    this->systemCtrl.load(scene.getCam(), scene.getFrame(), playerData, floorData);
    this->systemCtrl.start();
    
    scene.interface->registerIndex(playerData->ctrlIndex, [playerData](KeyCode code) {
        return playerData->KeyCode2KeyType(code);
    });
    
    scene.interface->registerIndex(playerData->debugIndex, [](KeyCode code) {
        switch(code)
        {
            case KeyCode::KEY_K: return CtrlKeyType::down;
            case KeyCode::KEY_I: return CtrlKeyType::up;
            case KeyCode::KEY_L: return CtrlKeyType::right;
            case KeyCode::KEY_J: return CtrlKeyType::left;
            default: return CtrlKeyType::none;
        }
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
    
    this->registerDispatcher(scene);
}

void MissionMediator::registerDispatcher(MissionScene& scene)
{
    this->systemRegs.clear();
    auto& dispatcher = this->systemCtrl.getDispatcher();
    auto floorData = GameCtrl::instance()->getData().curFloor();
    auto playerData = GameCtrl::instance()->getData().curPlayer();
    
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
    
    this->systemRegs.push_back(scene.interface->onSetActionMode.registerObserver(
        [this, &dispatcher](ActionMode mode){
            dispatcher.onSelectionAction(mode);
    }));
    
    this->systemRegs.push_back(scene.getCam()->onTouch.registerObserver(
        [this, &dispatcher, floorData](cc::Point pos){
            auto localPos = pos - floorData->getCurrentRoom()->getBounds().origin;
            dispatcher.onSelectionPos(localPos);
    }));
    
    this->systemRegs.push_back(scene.getCam()->onSwipe.registerObserver(
        [this, &dispatcher, floorData](cc::Point pos1, cc::Point pos2){
            auto localPos1 = pos1 - floorData->getCurrentRoom()->getBounds().origin;
            auto localPos2 = pos2 - floorData->getCurrentRoom()->getBounds().origin;
            Log("swipe=%f,%f -> %f,%f", localPos1.x, localPos1.y, localPos2.x, localPos2.y);
    }));
    
    //system events
    this->systemRegs.push_back(dispatcher.onHealthChanged.registerObserver(
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
    
    this->systemRegs.push_back(dispatcher.onGateTriggered.registerObserver(
        [this](unsigned prevRoomIndex, unsigned eid, GateMap  gate) {
            if (eid != GameCtrl::instance()->getData().curPlayer()->entityFocus)
                return;
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
    
    this->systemRegs.push_back(dispatcher.onGearChanged.registerObserver(
        [this, playerData, &scene](unsigned eid, const cp::GearComponent& gear) {
            if (playerData->entityFocus == eid)
            {
                scene.interface->getInventoryPanel()->updatePlayer(eid, gear);
            }
    }));
}

void MissionMediator::onRemoveView(MissionScene &scene)
{
    systemCtrl.clear();
}

void MissionMediator::onTick(double dt)
{
    systemCtrl.tick(dt);
}

void MissionMediator::onAnimate(double dt, double tickPercent)
{
    systemCtrl.animate(dt, tickPercent);
}