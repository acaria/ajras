#include "GameCtrl.h"
#include "MapData.h"
#include "GameScene.h"
#include "Randgine.h"

using namespace std::placeholders;

GameCtrl::GameCtrl():log(), scene(log),
                     tick(std::bind(&GameCtrl::onTick, this, _1),
                          std::bind(&GameCtrl::onAnimate, this, _1, _2)),
                     currentMap(nullptr)
{
    log.setLevelMask(LL_DEBUG | LL_INFO | LL_TRACE | LL_WARNING | LL_ERROR | LL_FATAL);
    log.setTimeoutSeconds(15);
    this->sessionEnabled = false;
}

GameCtrl::~GameCtrl()
{
    this->destroyMap();
}

void GameCtrl::start()
{
    //todo: loader
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ss-main.plist");
    
    this->scene.go2MainMenu();
}

void GameCtrl::scheduleUpdate(cocos2d::Node* parent)
{
    tick.schedule(parent);
}

void GameCtrl::tickUpdate(float dt)
{
    tick.update(dt);
}

void GameCtrl::onTick(double dt)
{
    if (sessionEnabled)
    {
        floorSystemCtrl.tick(dt);
    }
}

void GameCtrl::onAnimate(double dt, double tickPercent)
{
    if (sessionEnabled)
    {
        floorSystemCtrl.animate(dt, tickPercent);
    }
}

void GameCtrl::loadSession(GameScene* view)
{
    /*auto map = floorSystemCtrl.displayMap(this->currentMap);
    auto currentRoom = this->currentMap->getRoomAt(this->currentMap->getCurIdxRoom());
    auto startPos = currentRoom->position * -0.1;
    map->setAnchorPoint({0,0});
    map->setScale(0.2);
    map->setPosition(80,0);
    view->frame->addChild(map);
    return;
    */
    
    floorSystemCtrl.load(view, this->currentMap);
    floorSystemCtrl.start();
    tick.schedule(view);
    this->sessionEnabled = true;
}

void GameCtrl::destroyMap()
{
    if (this->currentMap != nullptr)
    {
        delete this->currentMap;
        this->currentMap = nullptr;
    }
    
    profileModel.clear();
}

void GameCtrl::newSession()
{
    this->sessionEnabled = false;
    this->destroyMap();
    
    Randgine::instance()->setMaster(1);
    
    this->currentMap = MapData::generate("md1-1");
    assert(currentMap);
    
    //todo: loader
    for(auto ss : this->currentMap->getSriteSheets())
    {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(
            "ss-" + ss + ".plist");
    }
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ss-gui.plist");
    
    this->scene.go2Game();
}
