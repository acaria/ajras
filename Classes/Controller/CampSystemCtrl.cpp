#include "CampSystemCtrl.h"
#include "Components.h"
#include "CampData.h"
#include "PlayerData.h"

CampSystemCtrl::CampSystemCtrl() : renderSystem(ecsGroup),
        collisionSystem(ecsGroup),
        moveSystem(ecsGroup),
        transSystem(ecsGroup),
        inputSystem(ecsGroup),
        meleeSystem(ecsGroup),
        targetSystem(ecsGroup),
        aiSystem(ecsGroup),
        interactSystem(ecsGroup),
        controlSystem(ecsGroup),
        random(Randgine::instance()->get(Randgine::CAMP))
{
    this->eventRegs.push_back(meleeSystem.onHealthChanged.registerObserver(
            [this](unsigned eid, int health) {
    }));
    this->eventRegs.push_back(transSystem.onGateTriggered.registerObserver(
            [this](unsigned eid, GateMap gate) {
    }));
}

CampSystemCtrl::~CampSystemCtrl()
{
    
}

void CampSystemCtrl::tick(double dt)
{
    controlSystem.tick(dt);
    
    
}

void CampSystemCtrl::animate(double dt, double tickPercent)
{
    controlSystem.animate(dt, tickPercent);
}

ControlSystem* CampSystemCtrl::getCtrlSystem()
{
    return &this->controlSystem;
}

void CampSystemCtrl::start()
{
    
}

void CampSystemCtrl::load(GameCamera *cam, cc::Node *view,
                          PlayerData *player, CampData *data)
{
    this->view = view;
    this->mapView = LayeredNode::create();
    this->view->addChild(mapView);
    
    this->cam = cam;
    this->data = data;
    this->playerData = player;
    this->controlSystem.init(player);
    
    auto grid = data->getContent();
    
    for(unsigned j = 0; j < grid.height; j++)
    for(unsigned i = 0; i < grid.width; i++)
    {
        auto properties = grid.get(i, j).fields;
        if (properties.find(BlockInfo::bgTileName) != properties.end())
        {
            //HACK: put most of tiles in bg layer
            auto rl = mapView->bg;
            if (properties.find(BlockInfo::collision) != properties.end() &&
                properties[BlockInfo::collision] == "flyable")
            {
                rl = mapView->main;
            }
            
            auto coord = data->getPosFromCoord({i,j});
            auto sprite = cc::Sprite::createWithSpriteFrameName(
                    properties[BlockInfo::bgTileName]);
            sprite->getTexture()->setAntiAliasTexParameters();
            sprite->setAnchorPoint({0, 0});
            sprite->setPosition(coord);
            rl->addChild(sprite, data->getZOrder(coord));
        }
        
        if (properties.find(BlockInfo::fgTileName) != properties.end())
        {
            auto coord = data->getPosFromCoord({i,j});
            auto sprite = cc::Sprite::createWithSpriteFrameName(
                    properties[BlockInfo::fgTileName]);
            sprite->getTexture()->setAntiAliasTexParameters();
            sprite->setAnchorPoint({0, 0});
            sprite->setPosition(coord);
            mapView->fg->addChild(sprite, data->getZOrder(coord));
        }
    }
}