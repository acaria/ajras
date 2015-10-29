#pragma once

#include "Singleton.h"
#include "ScreenLog.h"
#include "TickCtrl.h"
#include "SceneManager.h"
#include "GameData.h"
#include "DebugInfoLayer.h"

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
    
    GameData& getData();

protected:
    SceneManager    scene;

private:
    void onTick(double dt);
    void onAnimate(double dt, double tickPercent);
    
    GameData gameData;
    
#if GINFO_DEBUG
    DebugInfoLayer* debugInfo;
#endif
};