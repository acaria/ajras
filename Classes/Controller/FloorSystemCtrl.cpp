#include "FloorSystemCtrl.h"
#include "Components.h"
#include "BlockInfo.h"
#include "LayeredContainer.h"
#include "FloorData.h"
#include "RoomData.h"
#include "MissionScene.h"
#include "MissionInterface.h"
#include "NodeRenderer.h"
#include "HealthBar.h"
#include "ModelProvider.h"
#include "RenderComponent.h"
#include "PlayerData.h"
#include "CmdFactory.h"

using namespace std::placeholders;

FloorSystemCtrl::FloorSystemCtrl() : systemFacade(dispatcher, context, Randgine::CAT::MISSION)
{
    //init group
    this->context.ecs = &ecsGroup;

    //init systems
    this->systemFacade.factory<ControlSystem>(std::list<unsigned>(PlayerData::getCtrlIdxList()));
    this->systemFacade.factory<AISystem>();
    this->systemFacade.factory<UpdaterSystem>();
    this->systemFacade.factory<TargetSystem>();
    this->systemFacade.factory<MoveSystem>();
    this->systemFacade.factory<MeleeSystem>();
    this->systemFacade.factory<TransitSystem>();
    this->systemFacade.factory<CollisionSystem>();
    this->systemFacade.factory<RenderSystem>();
    this->systemFacade.factory<InteractSystem>();
#if ECSYSTEM_DEBUG
    this->systemFacade.factory<DebugSystem>();
#endif
}

FloorSystemCtrl::~FloorSystemCtrl()
{
    this->clear();
}

void FloorSystemCtrl::clear()
{
    this->eventRegs.clear();
    for(auto pair : this->roomSystems)
    {
        cp::entity::clear(pair.first);
        if (pair.second != nullptr)
        {
            delete pair.second;
        }
    }
    this->roomSystems.clear();
    this->roomViews.clear();
}

void FloorSystemCtrl::tick(double dt)
{
    systemFacade.tick(dt);
}

void FloorSystemCtrl::animate(double dt, double tickPercent)
{
    //focus entity
    if (playerData->getEntityFocusID() != 0 &&
        ecs::has<cp::Render>(playerData->getEntityFocusID()))
    {
        auto& cpRender = ecs::get<cp::Render>(playerData->getEntityFocusID());
        auto pos = this->data->getCurrentRoom()->position +
            cpRender.sprite->getPosition() + cpRender.sprite->getContentSize() / 2;
        this->cam->focusTarget(pos);
    }

    systemFacade.animate(dt, tickPercent);
}

SystemDispatcher& FloorSystemCtrl::getDispatcher()
{
    return dispatcher;
}

void FloorSystemCtrl::showEntityFromGate(unsigned roomIndex, unsigned eid,
                                         const GateMap& gate, float duration)
{
    auto roomData = data->getRoomAt(roomIndex);
    
    auto& render = ecs::get<cp::Render>(eid);
    auto animPos = roomData->extractGateAnimInfo(gate, ecs::get<cp::Physics>(eid).shape);
    render.sprite->setPosition(animPos.first);
    render.sprite->runAction(cc::Sequence::create(
        cc::MoveBy::create(duration, animPos.second - animPos.first),
        cc::CallFunc::create([eid, roomIndex, this, &render](){
            SysHelper::enableEntity(roomIndex, eid);
            dispatcher.onEntityAdded(roomIndex, eid);
        }),
        NULL
    ));
    render.sprite->runAction(cc::Sequence::create(
        cc::DelayTime::create(duration / 2),
        cc::Spawn::create(
            cc::TintTo::create(duration / 2, cc::Color3B::WHITE),
            cc::FadeTo::create(duration / 4, 255),
            NULL),
        NULL
    ));
}

void FloorSystemCtrl::changeEntityRoom(unsigned prevRoomIndex, unsigned eid,
                                       const GateMap& gate)
{
    unsigned nextRoomIndex = gate.destRoomIndex;

    dispatcher.onEntityDeleted(prevRoomIndex, eid);
    cp::entity::move(eid, prevRoomIndex, nextRoomIndex);

    if (ecs::has<cp::Render>(eid))
    {
        auto& sprite = ecs::get<cp::Render>(eid).sprite;
        auto container = this->roomViews[nextRoomIndex];
        if (sprite->getParent() != nullptr)
        {
            auto lType = static_cast<def::LayerType>(sprite->getParent()->getTag());
            sprite->removeFromParentAndCleanup(false);
            container->add(sprite, lType);
        }
        else //orphelin, cannot retrieve layertype
        {
            container->add(sprite, def::LayerType::BG);
        }
    }
    
    this->showEntityFromGate(nextRoomIndex, eid, gate, 1.0);
    
    if (eid == playerData->getEntityFocusID()) //change room
    {
        auto nextRoom = data->getRoomAt(nextRoomIndex);
        
        auto animPos = nextRoom->extractGateAnimInfo(gate, ecs::get<cp::Physics>(eid).shape);
        this->switchRoom(prevRoomIndex, nextRoomIndex, eid, animPos.second);
    }
}

void FloorSystemCtrl::switchRoom(unsigned fromRoomIndex, unsigned toRoomIndex,
                                 unsigned eid, cc::Vec2 destPos)
{
    this->roomSystems[fromRoomIndex]->hideObjects(1);
    dispatcher.onSystemChanged(toRoomIndex);
    this->data->setCurIdxRoom(toRoomIndex);
    
    auto dataRoom = data->getRoomAt(toRoomIndex);
    
    //move camera
    auto bounds = dataRoom->getBounds();
    this->cam->moveTarget(destPos + bounds.origin, 1);

    this->showRoom(toRoomIndex);
    
    CmdFactory::lightCfg(context.ecs, 0.5,
                         def::shader::LightParam::brightness,
                         data->getLightConfig().brightness);
    CmdFactory::lightCfg(context.ecs, 0.5,
                         def::shader::LightParam::cutOffRadius,
                         data->getLightConfig().cutOffRadius);
}

/*cc::Sprite* FloorSystemCtrl::displayMap(FloorData *data)
{
    auto result = cc::Sprite::create();
    
    auto pxl = cc::Sprite::createWithSpriteFrameName("pixel.png");
    pxl->setAnchorPoint({0,0});
    pxl->setScale(data->floorMapping->getMappingSize().width / 4,
                  data->floorMapping->getMappingSize().height / 4);
    pxl->setColor(cc::Color3B::ORANGE);
    result->addChild(pxl);
    
    for(auto pair : data->rooms)
    {
        auto dataRoom = pair.second;
        for(auto wall : dataRoom->getWalls())
        {
            auto pxl = cc::Sprite::createWithSpriteFrameName("pixel.png");
            pxl->setAnchorPoint({0,0});
            pxl->setPosition({(dataRoom->position.x + wall.origin.x) / 4,
                (dataRoom->position.y + wall.origin.y) / 4});
            pxl->setScale(wall.size.width / 4, wall.size.height / 4);
            pxl->setColor(cc::Color3B::RED);
            result->addChild(pxl);
        }
        
        for(auto gate : dataRoom->gateMapping)
        {
            auto gateMap = gate.second;
            auto pxl = cc::Sprite::createWithSpriteFrameName("pixel.png");
            pxl->setAnchorPoint({0,0});
            pxl->setPosition({
                (dataRoom->position.x + gateMap.info.rect.origin.x) / 4,
                (dataRoom->position.y + gateMap.info.rect.origin.y) / 4});
            pxl->setScale(gateMap.info.rect.size.width / 4, gateMap.info.rect.size.height / 4);
            if (gateMap.cmd == GateMap::CmdType::ENTER_MAP)
                pxl->setColor(cc::Color3B::BLACK);
            else if (gateMap.cmd == GateMap::CmdType::EXIT_MAP)
                pxl->setColor(cc::Color3B::BLUE);
            else
                pxl->setColor(cc::Color3B::MAGENTA);
            result->addChild(pxl);
        }
        
        auto txt = cc::Label::createWithTTF(std::to_string(dataRoom->depth), "fonts/04b03.ttf", 8);
        txt->setColor(cc::Color3B::GREEN);
        txt->setPosition({dataRoom->getBounds().getMidX() / 4,
                          dataRoom->getBounds().getMidY() / 4});
        result->addChild(txt);
    }

    return result;
}

void FloorSystemCtrl::displayDebug(cc::Node* view, FloorData *data)
{
    for(auto pair : data->rooms)
    {
        //auto roomIndex = pair.first;
        auto roomData = pair.second;
        auto bounds = roomData->getBounds();
        
        auto grid = roomData->getContent();
        
        for(int j = 0; j < grid.height; j++)
        for(int i = 0; i < grid.width; i++)
        {
            auto gridInfo = grid.get(i, j);
            auto pxl = cc::Sprite::createWithSpriteFrameName("pixel.png");
            pxl->setPosition({
                bounds.getMinX() / 4 + i * 4,
                bounds.getMinY() / 4 + j * 4
            });
            pxl->setScale(4.0, 4.0);
            pxl->setColor(cc::Color3B::BLACK);
            
            if (gridInfo.fields[BlockInfo::collision] == "walkable" ||
                gridInfo.fields[BlockInfo::collision] == "flyable")
            {
                if (roomData->type == RoomData::RoomType::STARTER)
                    pxl->setColor(cc::Color3B::RED);
                else if (roomData->type == RoomData::RoomType::FINISHER)
                    pxl->setColor(cc::Color3B::GREEN);
                else
                    pxl->setColor(cc::Color3B::YELLOW);
            }
            
            view->addChild(pxl);
        }
        
        auto txt = cc::Label::createWithTTF(std::to_string(roomData->depth), "fonts/04b03.ttf", 8);
        txt->setColor(cc::Color3B::GREEN);
        txt->setPosition({bounds.getMidX() / 4,
                          bounds.getMidY() / 4});
        view->addChild(txt);
    }
}*/

void FloorSystemCtrl::showRoom(unsigned int roomIndex, std::function<void()> after)
{
    assert(lib::hasKey(this->roomViews, roomIndex));
    
    if (!lib::hasKey(this->roomPreviews, roomIndex))
    {
        this->roomSystems[roomIndex]->showObjects(0.5);
        if (after != nullptr)
            after();
    }
    else
    {
        auto preview = this->roomPreviews[roomIndex];
        this->roomPreviews.erase(roomIndex);
        
        auto view = this->roomViews[roomIndex];
        
        preview->getSprite()->runAction(cc::Sequence::create(
            cc::FadeTo::create(1, 255),
            cc::CallFunc::create([this, view, preview, roomIndex](){
                this->view->removeChild(preview);
                this->view->addChild(view);
                view->release();
                this->roomSystems[roomIndex]->showObjects(0.5);
            }),
            cc::CallFunc::create(after),
            NULL
        ));
    }
}

void FloorSystemCtrl::bindSystems()
{
    //bind events
    this->eventRegs.clear();
    this->eventRegs.push_back(dispatcher.onGateTriggered.registerObserver(
            [this](unsigned prevRoomIndex, unsigned eid, GateMap  gate){
        switch(gate.cmd)
        {
            case GateMap::CmdType::CHANGE_ROOM:
                this->changeEntityRoom(prevRoomIndex, eid, gate);
                break;
            default:
                break;
        }
    }));
    
    this->eventRegs.push_back(dispatcher.onGateEnter.registerObserver(
    [this](unsigned prevRoomIndex, unsigned eid, GateMap  gate){
        switch(gate.cmd)
        {
            case GateMap::CmdType::CHANGE_ROOM:
                if (eid == playerData->getEntityFocusID()) //change room
                {
                    CmdFactory::lightCfg(context.ecs, 0.5,
                                         def::shader::LightParam::brightness, 0);
                    CmdFactory::lightCfg(context.ecs, 0.5,
                                         def::shader::LightParam::cutOffRadius, 0);
                }
                break;
            default:
                break;
        }
    }));
    
    this->eventRegs.push_back(dispatcher.onSystemChanged.registerObserver(
            [this](unsigned group) {
        context.data = this->data->rooms[group];
        context.view = roomViews[group];
        context.ecs->setID(group);
        dispatcher.onContextChanged();
    }));
}

void FloorSystemCtrl::loadLevel()
{
    cc::Rect bounds = cc::Rect::ZERO;
    for(auto pair : data->rooms)
    {
        auto roomIndex = pair.first;
        auto roomData = pair.second;
        
        auto layeredNode = cc::create<LayeredContainer>(roomData->getBounds().size);
        layeredNode->retain();
        
        auto roomSystemCtrl = new RoomSystemCtrl(roomIndex, layeredNode, roomData, dispatcher);
        
        roomSystemCtrl->hideObjects(0);
        
        this->roomViews[roomIndex] = layeredNode;
        this->roomSystems[roomIndex] = roomSystemCtrl;
        
        auto preview = layeredNode->createShot(roomData->getBounds().size.width,
                                               roomData->getBounds().size.height);
        this->roomPreviews[roomIndex] = preview;
        
        layeredNode->setPosition(roomData->position);
        preview->setPosition(roomData->position);
        preview->getSprite()->setOpacity(0);
        
        this->view->addChild(preview, 1);
        preview->setOpacity(0);
        
        bounds = bounds.unionWithRect(roomData->getBounds());
    }
    
    cam->setFrameBounds(bounds);
    
    cc::Node* bgLayer;
    if (data->getTextures().size() > 0) //back texture
    {
        auto& rand = Randgine::instance()->get(Randgine::CAT::MISSION);
        auto bgTiles = data->getBgTiles();
        auto texName = rand.select(bgTiles);
        
        auto tex = cc::Director::getInstance()->getTextureCache()->getTextureForKey(texName);
        assert(tex != nullptr);
        cc::Texture2D::TexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
        tex->setTexParameters(tp);
        bgLayer = cc::Sprite::createWithTexture(tex,
                                                {0,0,bounds.size.width, bounds.size.height});
    }
    else //back color
    {
        bgLayer = cc::LayerColor::create(cc::Color4B(data->getBgColor()),
                                         bounds.size.width, bounds.size.height);
    }
    bgLayer->setAnchorPoint({0,0});
    bgLayer->setPosition(bounds.origin);
    view->addChild(bgLayer, 0);
}

void FloorSystemCtrl::loadEntities()
{
    auto roomIndex = this->data->getCurIdxRoom();
    auto camRect = data->getCurrentRoom()->getBounds();
    this->cam->setTarget({camRect.getMidX(), camRect.getMidY()});
    
    
    auto roomData = this->data->rooms[roomIndex];
    auto roomView = this->roomViews[roomIndex];
    
    //find warp enter
    GateMap enterGate = roomData->getEnterGate();
    
    //show room
    this->showRoom(roomIndex, [this, enterGate, roomIndex]() {
        unsigned focusID = playerData->getEntityFocusID();
        
        auto duration = 3.0;
        if (focusID != 0 && ecs::has<cp::Render, cp::Physics>(focusID))
        {
            auto& cpRender = ecs::get<cp::Render>(focusID);
            cpRender.sprite->stopAllActions();
            
            this->showEntityFromGate(roomIndex, focusID, enterGate, duration);
           
            cpRender.sprite->setOpacity(0);
            cpRender.busy = true;
            cpRender.setMoveAnimation(enterGate.info.getDir(), true);
            
            cpRender.sprite->runAction(cc::Sequence::create(
                cc::DelayTime::create(duration),
                cc::CallFunc::create([focusID, this](){
                    auto& cpRender = ecs::get<cp::Render>(focusID);
                    cpRender.cancelAnimation();
                    context.ecs->add<cp::Position>(focusID).set(cpRender.sprite->getPosition());
                    dispatcher.onEntityPositionChanged(context.ecs->getID(), focusID);
                    CmdFactory::lightCfg(context.ecs, 0.5,
                                         def::shader::LightParam::brightness,
                                         data->getLightConfig().brightness);
                    CmdFactory::lightFollow(context.ecs, focusID, {0,0});
                }),
                NULL
            ));
        }
    });
    
    //create player entities
    for(auto& playerEntity : playerData->entities)
    {
        auto eid = SysHelper::createPlayerEntity(roomView,
                                                 roomIndex,
                                                 enterGate.info.getSrcPos(), playerEntity);
        playerEntity.entityID = eid;
        SysHelper::disableEntity(roomIndex, eid);
        dispatcher.onEntityAdded(roomIndex, eid);
    }
}

void FloorSystemCtrl::load(GameCamera *cam, cc::Node *view,
                           PlayerData *player, FloorData *data)
{
    //init vars
    this->view = view;
    this->cam = cam;
    this->data = data;
    this->playerData = player;
    
    this->loadLevel();
    
    //init context
    this->ecsGroup.setID(data->getCurIdxRoom());
    this->context.data = this->data->getCurrentRoom();
    this->context.view = this->roomViews[this->data->getCurIdxRoom()];
    this->bindSystems();
    //systems READY
    dispatcher.onContextChanged();
    
    this->loadEntities();
}