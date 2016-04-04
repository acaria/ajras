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
    logger.setFontFile(def::font::romulus);
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

    //load shaders
    shaders.load();
}

GameCtrl::~GameCtrl()
{
}

void GameCtrl::start()
{
    //CocosHelper::addTexture("ss-main.plist", def::antialias);
    //this->scene.go2Test();
    
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
    
    CocosHelper::addSpriteSheet("ss-main.plist", def::antialias);
    this->scene.go2MainMenu();
}

void GameCtrl::goToCamp(std::string startKey)
{
    this->gameData.loadCamp();
    this->gameData.getPlayer()->startCampKey = startKey;
    
    for(auto ss : this->gameData.getCamp()->getSpriteSheets())
        CocosHelper::addSpriteSheet("ss-" + ss + ".plist", def::antialias);
    
    this->scene.go2Camp();
}

void GameCtrl::goToMission()
{
    this->gameData.loadMission();
    
    for(auto ss : this->gameData.getFloor()->getSpriteSheets())
        CocosHelper::addSpriteSheet("ss-" + ss + ".plist", def::antialias);
    for(auto tex : this->gameData.getFloor()->getTextures())
        CocosHelper::addTexture(tex, def::antialias);
    CocosHelper::addSpriteSheet("ss-gui_mission.plist", def::antialias);
    
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

GameData* GameCtrl::data()
{
    return &GameCtrl::instance()->gameData;
}

LightManager* GameCtrl::light()
{
    return &GameCtrl::instance()->lightManager;
}
