#pragma once
#include "Headers.h"

class GameCtrl : public Singleton<GameCtrl>
{
public:
    GameCtrl();
    ~GameCtrl();
    
    void start();
    void newSession();
    void loadSession(GameScene* view);
    
    RoomData* changeRoom(unsigned roomIndex, unsigned gateIndex, const std::vector<unsigned> &eids);

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
};