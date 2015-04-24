#pragma once

#include "Singleton.h"
#include "ScreenLog.h"
#include "TickCtrl.h"
#include "ProfileModel.h"
#include "BehaviourModel.h"
#include "SceneManager.h"
#include "FloorSystemCtrl.h"

class RoomData;
class MapData;
class GameScene;

class GameCtrl : public Singleton<GameCtrl>
{
public:
    GameCtrl();
    ~GameCtrl();
    
    void start();
    void newSession();
    void loadSession(GameScene* view);

    ScreenLog       log;

    TickCtrl        tick;
    
    //models
    ProfileModel    profileModel;
    BehaviourModel  behaviourModel;

    void tickUpdate(float dt);
    void scheduleUpdate(cocos2d::Node* parent);

protected:
    SceneManager    scene;

private:
    void onTick(double dt);
    void onAnimate(double dt, double tickPercent);
    void destroyMap();
    
    MapData*        currentMap;
    
    FloorSystemCtrl  floorSystemCtrl;
    bool sessionEnabled;
};