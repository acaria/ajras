#include "MissionMediator.h"
#include "GameCtrl.h"

void MissionMediator::onAddView(MissionScene &scene)
{
    auto floorData = GameCtrl::instance()->getData().curFloor();
    auto playerData = GameCtrl::instance()->getData().getPlayerData();
    
    scene.setBgColor(floorData->getBgColor());
    
    //reset light
    GameCtrl::instance()->getEffects().setLightParam(def::shader::LightParam::brightness, 0);
    GameCtrl::instance()->getEffects().setLightParam(def::shader::LightParam::cutOffRadius, 0);
    
    this->systemCtrl.load(scene.getCam(), scene.getFrame(), playerData, floorData);
    
    if (playerData->entities.size() > 0) //take only the leader (first one)
    {
        auto playerEntity = playerData->entities.front();
        scene.interface->getInventoryPanel()->registerPlayer(playerEntity.entityID,
                                                             playerEntity.inventory);
    }
    
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
    auto playerData = GameCtrl::instance()->getData().getPlayerData();
    
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
            [&scene, playerData, &dispatcher](KeyCode code){
        auto pair = playerData->KeyCode2KeyType(code);
        if (pair.first != 0 || pair.second == CtrlKeyType::none)
        {
            switch(pair.second)
            {
                case CtrlKeyType::sel1:
                    scene.interface->setActionMode(ActionMode::team);
                    break;
                case CtrlKeyType::sel2:
                    scene.interface->setActionMode(ActionMode::inventorize);
                    break;
                case CtrlKeyType::sel3:
                    scene.interface->setActionMode(ActionMode::map);
                    break;
                case CtrlKeyType::down:
                case CtrlKeyType::up:
                case CtrlKeyType::left:
                case CtrlKeyType::right:
                {
                    Dir dir = Dir::fromCtrlKeyType(pair.second);
                    
                    if (dir != Dir::None) //key direction
                        dispatcher.onKeyPressDirection(pair.first, dir.getRaw());
                    break;
                }
                default: break;
            }
            
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
            if (eid != GameCtrl::instance()->getData().getPlayerData()->getEntityFocusID())
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
            if (playerData->getEntityFocusID() == eid)
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