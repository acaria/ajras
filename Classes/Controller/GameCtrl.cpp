#include "GameCtrl.h"
#include "FloorData.h"
#include "MissionScene.h"
#include "Randgine.h"
#include "PlayerData.h"

using namespace std::placeholders;

GameCtrl::GameCtrl():tick(std::bind(&GameCtrl::onTick, this, _1),
                          std::bind(&GameCtrl::onAnimate, this, _1, _2)),
                     scene(tick)

{
    log.setLevelMask(LL_DEBUG | LL_INFO | LL_TRACE | LL_WARNING | LL_ERROR | LL_FATAL);
    log.setTimeoutSeconds(15);
}

GameCtrl::~GameCtrl()
{
}

void GameCtrl::start()
{
    this->goToMainMenu();
}

void GameCtrl::goToMainMenu()
{
    this->gameData.reset();
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ss-main.plist");
    this->scene.go2MainMenu();
}

void GameCtrl::goToMission()
{
    //engine ---
    Randgine::instance()->setMaster(1);
    
    this->gameData.loadPlayer();
    this->gameData.loadMission();
    
    for(auto ss : this->gameData.curFloor()->getSriteSheets())
    {
        cc::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ss-" + ss + ".plist");
    }
    cc::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ss-gui.plist");
    
    this->scene.go2Mission();
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
    scene.onTick(dt);
}

void GameCtrl::onAnimate(double dt, double tickPercent)
{
    scene.onAnimate(dt, tickPercent);
}

/*void GameCtrl::startSession(MissionScene* view)
{
    auto map = floorSystemCtrl.displayMap(this->currentMap);
    auto currentRoom = this->currentMap->getRoomAt(this->currentMap->getCurIdxRoom());
    auto startPos = currentRoom->position * -0.1;
    map->setAnchorPoint({0,0});
    map->setScale(0.2);
    map->setPosition(80,0);
    view->frame->addChild(map);
    return;
}*/

GameData& GameCtrl::getData()
{
    return this->gameData;
}
