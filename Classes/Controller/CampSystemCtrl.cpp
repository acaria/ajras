#include "CampSystemCtrl.h"
#include "Components.h"
#include "CampData.h"
#include "PlayerData.h"
#include "ModelProvider.h"
#include "WarpMap.h"

CampSystemCtrl::CampSystemCtrl() :
        random(Randgine::instance()->get(Randgine::CAMP)),
        systemFacade(dispatcher, context, random)
{
    //init group
    ecsGroup.setID(GROUP_INDEX);
    this->context.ecs = &ecsGroup;
    
    //init systems
    this->systemFacade.factory<ControlSystem>(std::list<unsigned>({PlayerData::ctrlIndex}));
    this->systemFacade.factory<AISystem>();
    this->systemFacade.factory<UpdaterSystem>();
    this->systemFacade.factory<TargetSystem>();
    this->systemFacade.factory<MoveSystem>();
    this->systemFacade.factory<CollisionSystem>();
    this->systemFacade.factory<MeleeSystem>();
    this->systemFacade.factory<TransitSystem>();
    this->systemFacade.factory<HealthSystem>();
    this->systemFacade.factory<RenderSystem>();
    this->systemFacade.factory<InteractSystem>();
#if ECSYSTEM_DEBUG
    this->systemFacade.factory<DebugSystem>();
#endif
}

CampSystemCtrl::~CampSystemCtrl()
{
    this->clear();
}

void CampSystemCtrl::clear()
{
    this->eventRegs.clear();
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
    auto& cpPhy = ecsGroup.add<cp::Physics>(eid);
    
    cpRender.setProfile(profile, this->mapView);
    cpPhy.setProfile(profile);
    
    cc::Vec2 pos = {
        srcPos.x - cpPhy.shape.getMidX(),
        srcPos.y - cpPhy.shape.getMidY()
    };
    
    cpRender.sprite->setPosition(pos);
    cpRender.sprite->setOpacity(0);
    
    ecsGroup.add<cp::AI>(eid).setProfile(profile);
    ecsGroup.add<cp::Melee>(eid).setProfile(profile);
    ecsGroup.add<cp::Control>(eid) = playerData->ctrlIndex;
    ecsGroup.add<cp::Gear>(eid) = playerData->inventory;
    ecsGroup.add<cp::Health>(eid).setProfile(profile);
    ecsGroup.add<cp::Orientation>(eid);
    ecsGroup.add<cp::Input>(eid);
    
    playerData->entityFocus = eid;
    
    float duration = 3.0f;
    cpRender.setMoveAnimation(enterWarpRef->info.getDir(), true);
    
    cpRender.sprite->stopAllActions();
    cpRender.sprite->runAction(cc::Sequence::create(
        cc::MoveTo::create(duration, {
            destPos.x - cpPhy.shape.getMidX(),
            destPos.y - cpPhy.shape.getMidY()
        }),
        cc::CallFunc::create([eid, this, &cpRender](){
            context.ecs->add<cp::Position>(eid).set(cpRender.sprite->getPosition());
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
 
    //init context
    this->context.view = mapView;
    this->context.data = data;
    
    //systems READY
    dispatcher.onContextChanged();
}