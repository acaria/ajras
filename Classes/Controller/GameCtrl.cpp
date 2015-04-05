#include "Headers.h"

using namespace std::placeholders;

GameCtrl::GameCtrl():log(), scene(log),
                     tick(std::bind(&GameCtrl::onTick, this, _1),
                          std::bind(&GameCtrl::onAnimate, this, _1, _2)),
                     currentMap(nullptr)
{
    log.setLevelMask(LL_DEBUG | LL_INFO | LL_TRACE | LL_WARNING | LL_ERROR | LL_FATAL);
    log.setTimeoutSeconds(15);
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
    floorSystemCtrl.tick(dt);
}

void GameCtrl::onAnimate(double dt, double tickPercent)
{
    floorSystemCtrl.animate(dt, tickPercent);
}

void GameCtrl::loadSession(GameScene* view)
{
    floorSystemCtrl.load(view, this->currentMap);
    tick.schedule(view);
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

RoomData* GameCtrl::changeRoom(unsigned roomIndex, unsigned gateIndex, const std::vector<unsigned> &eids)
{
    return floorSystemCtrl.changeRoom(roomIndex, gateIndex, eids);
}

void GameCtrl::newSession()
{
    this->destroyMap();
    
    this->currentMap = MapData::generate("map1");
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
