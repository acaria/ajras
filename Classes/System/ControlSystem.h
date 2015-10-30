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

    void init(std::list<unsigned> indexes);
    
    //overrides
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
    void setSelectionAction(ActionMode mode);
    void setSelectionPos(cc::Point p);
    void setKeyPressAction(unsigned index, int flag);
    void setKeyReleaseAction(unsigned index, int flag);
    void setStickDirection(unsigned index, const lib::Nullable<cc::Vec2>& dir);
    
private:
    
    void clearReleased();
    
    //control input
    std::map<unsigned, unsigned>                 curDirPressed;
    std::map<unsigned, unsigned>                 curDirReleased;
    std::map<unsigned, unsigned>                 preDirPressed;
    std::map<unsigned, lib::Nullable<cc::Point>> joyPos;
    
    //selection input
    lib::Nullable<ActionMode> actionSelection = nullptr;
    lib::Nullable<cc::Point> posSelection = nullptr;
    
    std::list<unsigned> indexList;
};