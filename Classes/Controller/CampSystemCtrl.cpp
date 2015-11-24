#include "CampSystemCtrl.h"
#include "Components.h"
#include "CampData.h"
#include "PlayerData.h"
#include "ModelProvider.h"
#include "WarpMap.h"

CampSystemCtrl::CampSystemCtrl() :
        random(Randgine::instance()->get(Randgine::CAMP)),
        systemFacade(dispatcher, context)
{
    ecsGroup.setID(GROUP_INDEX);
}

CampSystemCtrl::~CampSystemCtrl()
{
    this->clear();
}

void CampSystemCtrl::clear()
{
    this->eventRegs.clear();
    this->systemFacade.clear();
    cp::entity::clear(this->ecsGroup.getID());
}

void CampSystemCtrl::tick(double dt)
{
    systemFacade.tick(dt);
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
    
    systemFacade.animate(dt, tickPercent);
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

    dispatcher.onEntityAdded(ecsGroup.getID(), eid);
}

SystemDispatcher& CampSystemCtrl::getDispatcher()
{
    return dispatcher;
}

void CampSystemCtrl::load(GameCamera *cam, cc::Node *view,
                          PlayerData *player, CampData *data)
{
    this->mapView = cc::create<LayeredContainer>(data->getBounds().size);
    view->addChild(mapView);
    
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
    }
    
    this->loadSystems(mapView, data);
}

void CampSystemCtrl::loadSystems(LayeredContainer* view, IMapData* data)
{
    //init context
    this->context.ecs = &ecsGroup;
    this->context.view = mapView;
    this->context.data = data;
    
    //init systems
    assert(this->systemFacade.count() == 0);
    this->systemFacade.factory<ControlSystem>(std::list<unsigned>({PlayerData::ctrlIndex}));
    this->systemFacade.factory<AISystem>();
    this->systemFacade.factory<UpdaterSystem>();
    this->systemFacade.factory<TargetSystem>();
    this->systemFacade.factory<MoveSystem>();
    this->systemFacade.factory<MeleeSystem>();
    this->systemFacade.factory<TransitSystem>();
    this->systemFacade.factory<CollisionSystem>();
    this->systemFacade.factory<HealthSystem>();
    this->systemFacade.factory<RenderSystem>();
    this->systemFacade.factory<InteractSystem>();
#if ECSYSTEM_DEBUG
    this->systemFacade.factory<DebugSystem>();
#endif

    //bind events
    this->eventRegs.clear();
    
    this->eventRegs.push_back(dispatcher.onEntityAdded.registerObserver(
            [this](unsigned group, unsigned eid) {
        if (ecs::has<cp::Position, cp::Physics>(eid))
            this->data->getCol()->agents[eid] = SysHelper::makeAgent(eid);
    }));
    
    this->eventRegs.push_back(dispatcher.onEntityDeleted.registerObserver(
            [this](unsigned group, unsigned eid) {
        this->data->getCol()->agents.erase(eid);
    }));

    dispatcher.onContextChanged();
}