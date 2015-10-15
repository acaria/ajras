#include "GameCtrl.h"
#include "FloorData.h"
#include "MissionScene.h"
#include "Randgine.h"
#include "Components.h"

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
    
    //this->goToMission();
    //this->goToCamp();
}

void GameCtrl::goToMainMenu()
{
    cp::entity::resetID();
    this->gameData.reset();
    
    Randgine::instance()->setMaster(1);
    this->gameData.loadPlayer();
    
    cc::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ss-main.plist");
    this->scene.go2MainMenu();
}

void GameCtrl::goToCamp()
{
    this->gameData.loadCamp();
    
    for(auto ss : this->gameData.curCamp()->getSpriteSheets())
    {
        cc::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ss-" + ss + ".plist");
    }
    
    this->scene.go2Camp();
}

void GameCtrl::goToMission()
{
    this->gameData.loadMission();
    
    for(auto ss : this->gameData.curFloor()->getSpriteSheets())
    {
        cc::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ss-" + ss + ".plist");
    }
    cc::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ss-gui_mission.plist");
    
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

GameData& GameCtrl::getData()
{
    return this->gameData;
}
