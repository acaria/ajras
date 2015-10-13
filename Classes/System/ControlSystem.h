#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"
#include "MissionInterface.h"
#include "PlayerData.h"

class RoomData;

class ControlSystem : public BaseTickSystem
{
public:
    //ctors
    ControlSystem(lib::EcsGroup& ecs);
    virtual ~ControlSystem() {}

    void init(PlayerData* player);
    
    //overrides
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
    void setSelectionAction(ActionMode mode);
    void setKeyPressAction(int flag);
    void setKeyReleaseAction(int flag);
    
    void setStickDirection(cc::Vec2 dir);
    void releaseStick();
    
private:
    
    void clearReleased();
    
    //control input
    unsigned    curDirPressed;
    unsigned    curDirReleased;
    unsigned    preDirPressed;
    
    bool        joyUpdate = false;
    cc::Vec2    joyPos;
    //selection input
    unsigned    entitySelection;
    ActionMode  actionSelection;
    
    PlayerData *player = nullptr;
};