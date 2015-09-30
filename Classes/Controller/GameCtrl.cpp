#include "GameCtrl.h"
#include "MapData.h"
#include "GameScene.h"
#include "Randgine.h"
#include "PlayerData.h"

using namespace std::placeholders;

GameCtrl::GameCtrl():log(), scene(log),
                     tick(std::bind(&GameCtrl::onTick, this, _1),
                          std::bind(&GameCtrl::onAnimate, this, _1, _2)),
                     currentMap(nullptr), P1(nullptr)
{
    log.setLevelMask(LL_DEBUG | LL_INFO | LL_TRACE | LL_WARNING | LL_ERROR | LL_FATAL);
    log.setTimeoutSeconds(15);
    this->sessionEnabled = false;
}

GameCtrl::~GameCtrl()
{
    this->cleanSession();
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

void GameCtrl::startSession(GameScene* view)
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
    
    tick.schedule(view);
    
    floorSystemCtrl.load(view, this->currentMap);
    floorSystemCtrl.start();
    this->sessionEnabled = true;
}

void GameCtrl::cleanSession()
{
    if (this->currentMap != nullptr)
    {
        delete this->currentMap;
        this->currentMap = nullptr;
    }
    
    if (P1 != nullptr)
    {
        delete this->P1;
        this->P1 = nullptr;
    }
    
    model.profile.clear();
}

PlayerData* GameCtrl::getP1()
{
    return this->P1;
}

void GameCtrl::newSession()
{
    this->sessionEnabled = false;
    this->cleanSession();
    
    //engine ---
    Randgine::instance()->setMaster(1);
    
    //player --
    this->P1 = new PlayerData();
    this->P1->ctrlIndex = 1;
    this->P1->keysDefList[CtrlKeyType::left] = {KeyCode::KEY_LEFT_ARROW, KeyCode::KEY_A};
    this->P1->keysDefList[CtrlKeyType::right] = {KeyCode::KEY_RIGHT_ARROW, KeyCode::KEY_D};
    this->P1->keysDefList[CtrlKeyType::up] = {KeyCode::KEY_UP_ARROW, KeyCode::KEY_W};
    this->P1->keysDefList[CtrlKeyType::down] = {KeyCode::KEY_DOWN_ARROW, KeyCode::KEY_S};
    this->P1->keysDefList[CtrlKeyType::autoselect] = {KeyCode::KEY_SPACE};
    this->P1->keysDefList[CtrlKeyType::sel1] = {KeyCode::KEY_1};
    this->P1->keysDefList[CtrlKeyType::sel2] = {KeyCode::KEY_2};
    this->P1->keysDefList[CtrlKeyType::sel3] = {KeyCode::KEY_3};
    
    //player inventory
    unsigned invCounter = 1;
    this->P1->inventory.push_back(new SlotData {
        .category = ColCat::head,
        .order = invCounter++,
        .content = nullptr
    });
    this->P1->inventory.push_back(new SlotData {
        .category = ColCat::object,
        .order = invCounter++,
        .content = nullptr
    });
    this->P1->inventory.push_back(new SlotData {
        .category = ColCat::object,
        .order = invCounter++,
        .content = nullptr
    });
    this->P1->inventory.push_back(new SlotData {
        .category = ColCat::object,
        .order = invCounter++,
        .content = nullptr
    });
    this->P1->inventory.push_back(new SlotData {
        .category = ColCat::object,
        .order = invCounter++,
        .content = nullptr
    });
    
    //map ---
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
