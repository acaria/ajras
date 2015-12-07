#include "GameCtrl.h"
#include "FloorData.h"
#include "MissionScene.h"
#include "Randgine.h"
#include "Components.h"
#include "CocosHelper.h"

using namespace std::placeholders;

GameCtrl::GameCtrl():tick(std::bind(&GameCtrl::onTick, this, _1),
                          std::bind(&GameCtrl::onAnimate, this, _1, _2)),
                     scene(&tick)

{
#if FOOTER_DEBUG
    logger.setLevelMask(LL_DEBUG | LL_INFO | LL_TRACE | LL_WARNING | LL_ERROR | LL_FATAL);
    logger.setFontFile(def::font::normal);
    logger.setTimeoutSeconds(15);
    
    this->eventRegs.push_back(scene.onSceneChanged.registerObserver([this](EventScene* scene){
        logger.attachToScene(scene);
    }));
#endif
    
#if HEADER_DEBUG
    this->debugHeader = cc::create<DebugInfoLayer>();
    this->debugHeader->retain();
    
    this->eventRegs.push_back(scene.onSceneChanged.registerObserver([this](EventScene* scene){
        this->debugHeader->removeFromParent();
        scene->addChild(this->debugHeader);
    }));
#endif
    
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
    
    Randgine::instance()->setMaster(2);
    this->gameData.loadPlayer();
    
    CocosHelper::addTexture("ss-main.plist", def::antialias);
    this->scene.go2MainMenu();
}

void GameCtrl::goToCamp(std::string startKey)
{
    this->gameData.loadCamp();
    this->gameData.curPlayer()->startCampKey = startKey;
    
    for(auto ss : this->gameData.curCamp()->getSpriteSheets())
    {
        CocosHelper::addTexture("ss-" + ss + ".plist", def::antialias);
    }
    
    this->scene.go2Camp();
}

void GameCtrl::goToMission()
{
    this->gameData.loadMission();
    
    for(auto ss : this->gameData.curFloor()->getSpriteSheets())
    {
        CocosHelper::addTexture("ss-" + ss + ".plist", def::antialias);
    }
    CocosHelper::addTexture("ss-gui_mission.plist", def::antialias);
    
    this->scene.go2Mission();
}

void GameCtrl::onTick(double dt)
{
#if HEADER_DEBUG
    this->debugHeader->update();
#endif

    auto mediator = scene.getCurMediator();
    if (mediator != nullptr)
        mediator->onTick(dt);
}

void GameCtrl::onAnimate(double dt, double tickPercent)
{
    auto mediator = scene.getCurMediator();
    if (mediator != nullptr)
        mediator->onAnimate(dt, tickPercent);
}

GameData& GameCtrl::getData()
{
    return this->gameData;
}
