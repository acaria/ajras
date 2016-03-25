#include "MissionMediator.h"
#include "GameCtrl.h"

void MissionMediator::onAddView(MissionScene &scene)
{
    auto floorData = GameCtrl::instance()->getData().curFloor();
    auto playerData = GameCtrl::instance()->getData().getPlayerData();
    
    scene.setBgColor(floorData->getBgColor());
    
    //init light
    GameCtrl::instance()->getLight().init(floorData->getLightConfig());
    
    this->systemCtrl.load(scene.getCam(), scene.getFrame(), playerData, floorData);
    
    for(auto entity : playerData->entities)
    {
        if (entity.ctrlIndex != 0 && ecs::has<cp::Gear>(entity.entityID)) //controlled entity, let binds inventory panel
        {
            //caution bind only from components to keep good refs!!!
            auto& gear = ecs::get<cp::Gear>(entity.entityID);
            scene.interface->setInventoryList(gear.currency, /*refs*/gear.slots);
        }
        scene.interface->addPlayerEntity(entity.entityID, entity.profileName,
                                         entity.health, entity.stamina);
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
                case CtrlKeyType::sel2:
                case CtrlKeyType::sel3:
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
        [&scene](unsigned roomIndex, unsigned int eid, float health) {
            scene.interface->updateHealthStatus(eid, health);
    }));
    
    this->systemRegs.push_back(dispatcher.onStaminaChanged.registerObserver(
        [&scene](unsigned roomIndex, unsigned int eid, float stamina) {
            scene.interface->updateStaminaStatus(eid, stamina);
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
    
    this->systemRegs.push_back(dispatcher.onGearSlotChanged.registerObserver(
            [this, playerData, &scene](unsigned eid, SlotData* slot) {
        if (playerData->getEntityFocusID() == eid)
        {
            scene.interface->updateInventorySlot(slot);
        }
    }));
    
    this->systemRegs.push_back(dispatcher.onGearCurrencyChanged.registerObserver(
            [this, playerData, &scene](unsigned eid, unsigned amount) {
        if (playerData->getEntityFocusID() == eid)
        {
            scene.interface->updateCurrencySlot(amount);
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