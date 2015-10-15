#pragma once

#include "Singleton.h"
#include "ScreenLog.h"
#include "TickCtrl.h"
#include "SceneManager.h"
#include "GameData.h"
class RoomData;
class FloorData;
class PlayerData;
class GameScene;

class GameCtrl : public Singleton<GameCtrl>
{
public:
    GameCtrl();
    ~GameCtrl();
    
    void start();

    void goToMainMenu();
    void goToCamp(std::string startKey = "home");
    void goToMission();

    ScreenLog       log;

    TickCtrl        tick;
    
    void tickUpdate(float dt);
    void scheduleUpdate(cocos2d::Node* parent);
    
    GameData& getData();

protected:
    SceneManager    scene;

private:
    void onTick(double dt);
    void onAnimate(double dt, double tickPercent);
    
    GameData gameData;
};