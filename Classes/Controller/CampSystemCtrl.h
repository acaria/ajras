#pragma once

class CampData;
class PlayerData;

#include "ControlSystem.h"
#include "Randgine.h"
#include "Event.h"
#include "GameCamera.h"

class CampSystemCtrl
{
public:
    CampSystemCtrl();
    ~CampSystemCtrl();
    
    void load(GameCamera* cam, cc::Node* view,
              PlayerData *player, CampData* data);
    void start();
    
    void tick(double dt);
    void animate(double dt, double tickPercent);
    
    ControlSystem* getCtrlSystem();
    
private:
    ControlSystem ctrlCampSystem;
    
    //local ecs
    lib::EcsGroup   ecsGroup;
    
    //view
    cc::Node* view = nullptr;
    GameCamera* cam = nullptr;
    
    //event
    std::vector<lib::Registration> eventRegs;
    
    lib::Random& random;
    
    PlayerData* playerData = nullptr;
};