#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"
#include "MissionInterface.h"
#include "PlayerData.h"
#include "CoreLib.h"

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
    void setSelectionPos(cc::Point p);
    void setKeyPressAction(int flag);
    void setKeyReleaseAction(int flag);
    void setStickDirection(const lib::Nullable<cc::Vec2>& dir);
    
private:
    
    void clearReleased();
    
    //control input
    unsigned    curDirPressed;
    unsigned    curDirReleased;
    unsigned    preDirPressed;
    lib::Nullable<cc::Point> joyPos = nullptr;
    
    //selection input
    lib::Nullable<ActionMode> actionSelection = nullptr;
    lib::Nullable<cc::Point> posSelection = nullptr;
    
    PlayerData *player = nullptr;
};