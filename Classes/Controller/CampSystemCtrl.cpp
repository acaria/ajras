#include "CampSystemCtrl.h"
#include "Components.h"
#include "CampData.h"
#include "PlayerData.h"
#include "ModelProvider.h"
#include "WarpMap.h"

CampSystemCtrl::CampSystemCtrl() : renderSystem(ecsGroup),
        collisionSystem(ecsGroup),
        moveSystem(ecsGroup),
        transSystem(ecsGroup),
        updaterSystem(ecsGroup),
        meleeSystem(ecsGroup),
        targetSystem(ecsGroup),
        aiSystem(ecsGroup),
        interactSystem(ecsGroup),
        controlSystem(ecsGroup),
        healthSystem(ecsGroup),
#if ECSYSTEM_DEBUG
        debugSystem(ecsGroup),
#endif
        random(Randgine::instance()->get(Randgine::CAMP))
{
    ecsGroup.setID(1);
    this->eventRegs.push_back(healthSystem.onHealthChanged.registerObserver(
            [this](unsigned eid, int health) {
        this->onHealthChanged(eid, health);
    }));
    this->eventRegs.push_back(transSystem.onWarpTriggered.registerObserver(
            [this](unsigned eid, WarpMap gate) {
        this->onWarpTriggered(eid, gate);
    }));
}

CampSystemCtrl::~CampSystemCtrl()
{
    this->clear();
}

void CampSystemCtrl::clear()
{
    cp::entity::clear(this->ecsGroup.getID());
}

void CampSystemCtrl::tick(double dt)
{
    controlSystem.tick(dt);
    aiSystem.tick(dt);
    updaterSystem.tick(dt);
    targetSystem.tick(dt);
    moveSystem.tick(dt);
    meleeSystem.tick(dt);
    transSystem.tick(dt);
    collisionSystem.tick(dt);
    healthSystem.tick(dt);
    renderSystem.tick(dt);
    interactSystem.tick(dt);
#if ECSYSTEM_DEBUG
    debugSystem.tick(dt);
#endif
}

void CampSystemCtrl::animate(double dt, double tickPercent)
{
    //focus entity
    if (playerData->entityFocus != 0 && ecs::has<cp::Render>(playerData->entityFocus))
    {
        auto& cpRender = ecs::get<cp::Render>(playerData->entityFocus);
        auto pos = cpRender.sprite->getPosition() + cpRender.sprite->getContentSize() / 2;
        this->cam->focusTarget(pos);
    }
    
    controlSystem.animate(dt, tickPercent);
    aiSystem.animate(dt, tickPercent);
    updaterSystem.animate(dt, tickPercent);
    targetSystem.animate(dt, tickPercent);
    moveSystem.animate(dt, tickPercent);
    meleeSystem.animate(dt, tickPercent);
    transSystem.animate(dt, tickPercent);
    collisionSystem.animate(dt, tickPercent);
    healthSystem.animate(dt, tickPercent);
    renderSystem.animate(dt, tickPercent);
    interactSystem.animate(dt, tickPercent);
#if ECSYSTEM_DEBUG
    debugSystem.animate(dt, tickPercent);
#endif
}

ControlSystem* CampSystemCtrl::getCtrlSystem()
{
    return &this->controlSystem;
}

void CampSystemCtrl::start()
{
    //find warp enter
    WarpMap* enterWarpRef = nullptr;
    for(auto element : this->data->warpMapping)
    {
        if (element.keyCmd == playerData->startCampKey)
        {
            enterWarpRef = &element;
            break;
        }
    }
    assert(enterWarpRef);
    auto camRect = enterWarpRef->info.rect;
    this->cam->setTarget({camRect.getMidX(), camRect.getMidY()});
    
    auto srcPos = enterWarpRef->info.getSrcPos();
    auto destPos = enterWarpRef->info.getDestPos();
    
    //create player
    auto eid = cp::entity::genID();
    auto profile = ModelProvider::instance()->profile.get(playerData->charProfileName);
    
    auto& cpRender = ecsGroup.add<cp::Render>(eid);
    auto& cpCollision = ecsGroup.add<cp::Physics>(eid);
    
    cpRender.setProfile(profile, this->mapView);
    cpCollision.setProfile(profile);
    
    ecsGroup.add<cp::AI>(eid).setProfile(profile);
    ecsGroup.add<cp::Melee>(eid).setProfile(profile);
    ecsGroup.add<cp::Control>(eid) = playerData->ctrlIndex;
    ecsGroup.add<cp::Gear>(eid) = playerData->inventory;
    ecsGroup.add<cp::Health>(eid).setProfile(profile);
    cpRender.sprite->setPosition({
        srcPos.x - cpCollision.shape.getMidX(),
        srcPos.y - cpCollision.shape.getMidY()
    });
    cpRender.sprite->setOpacity(0);
    cpRender.manualPosMode = true;
    ecsGroup.add<cp::Position>(eid).set(cpRender.sprite->getPosition());
    
    playerData->entityFocus = eid;
    
    float duration = 3.0f;
    cpRender.setMoveAnimation(enterWarpRef->info.getDir(), true);
    
    cpRender.sprite->stopAllActions();
    cpRender.sprite->runAction(cc::Sequence::create(
        cc::MoveTo::create(duration, {
            destPos.x - cpCollision.shape.getMidX(),
            destPos.y - cpCollision.shape.getMidY()
        }),
        cc::CallFunc::create([eid, this](){
            auto& cpRender = ecs::get<cp::Render>(eid);
            ecsGroup.add<cp::Input>(eid);
            ecs::get<cp::Position>(eid).set(cpRender.sprite->getPosition());
            cpRender.manualPosMode = false;
            cpRender.cancelAnimation();
        }),
        NULL
    ));
    cpRender.sprite->runAction(cc::Sequence::create(
        cc::DelayTime::create(duration / 2),
        cc::FadeTo::create(duration / 4, 255),
        NULL
    ));

}

void CampSystemCtrl::load(GameCamera *cam, cc::Node *view,
                          PlayerData *player, CampData *data)
{
    this->view = view;
    this->mapView = cc::create<LayeredContainer>(data->getBounds().size);
    this->view->addChild(mapView);
    
    this->cam = cam;
    this->data = data;
    this->playerData = player;
    
    auto& grid = data->getGrid();
    
    for(unsigned j = 0; j < grid.height; j++)
    for(unsigned i = 0; i < grid.width; i++)
    {
        auto properties = grid.get(i, j).fields;
        if (properties.find(BlockInfo::bgTileName) != properties.end())
        {
            //HACK: put most of tiles in bg layer
            auto rl = def::LayerType::BG;
            if (properties.find(BlockInfo::collision) != properties.end() &&
                properties[BlockInfo::collision] == "flyable")
            {
                rl = def::LayerType::MAIN1;
            }
            
            auto coord = data->getPosFromCoord({i,j});
            auto sprite = this->mapView->createChild(properties[BlockInfo::bgTileName], rl);
            sprite->setPosition(coord);
        }
        
        if (properties.find(BlockInfo::fgTileName) != properties.end())
        {
            auto coord = data->getPosFromCoord({i,j});
            auto sprite = this->mapView->createChild(properties[BlockInfo::fgTileName], def::LayerType::FG);
            sprite->setPosition(coord);
        }
    }
    
    //gates
    for(auto warpMap : data->warpMapping)
    {
        auto eid = cp::entity::genID();
        ecsGroup.add<cp::Warp>(eid) = warpMap;
        cc::Point pos = {warpMap.info.rect.origin.x, warpMap.info.rect.origin.y};
        
        //change collision data
        
        auto gateSrcCoord = data->getCoordFromPos(pos);
        auto gateDestCoord = data->getCoordFromPos({
            warpMap.info.rect.getMaxX(),
            warpMap.info.rect.getMaxY()});
        
        for(int j = gateSrcCoord.y; j < gateDestCoord.y; j++)
        for(int i = gateSrcCoord.x; i < gateDestCoord.x; i++)
        {
            grid.get(i, j).fields[BlockInfo::collision] = "walkable";
        }
    }
    //hack: twicks to generate after gates process 
    this->data->getCol()->process();
    
    //init systems
    this->controlSystem.init({PlayerData::ctrlIndex});
    this->updaterSystem.init(data);
    this->renderSystem.init(data);
    this->collisionSystem.init(data);
    this->interactSystem.init(data);
    this->meleeSystem.init(data);
    this->aiSystem.init(data);
    
#if ECSYSTEM_DEBUG
    this->debugSystem.init(this->mapView, data);
#endif
}