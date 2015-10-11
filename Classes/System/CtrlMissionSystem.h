#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"
#include "MissionInterfaceLayer.h"
#include "PlayerData.h"

class RoomData;

class CtrlMissionSystem : public BaseTickSystem
{
public:
    //ctors
    CtrlMissionSystem(lib::EcsGroup& ecs);
    virtual ~CtrlMissionSystem() {}

    void init(RoomData* data);
    void changeRoom(RoomData* data);
    
    //overrides
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
    void setSelectionAction(ActionMode mode);
    void setKeyPressAction(unsigned pindex, int flag);
    void setKeyReleaseAction(unsigned pindex, int flag);
    
    void setStickDirection(unsigned pindex, cc::Vec2 dir);
    void releaseStick(unsigned pindex);
    
    void setTouchFrameBegan(unsigned pindex, cc::Point camPos, cc::Point touchPos);
    
private:
    void initControl(unsigned index);
    
    void clearReleased(unsigned controlIndex);
    
    //internal methods
    static cc::Rect computeRect(cc::Point p1, cc::Point p2);
    
    //control input
    std::map<unsigned, unsigned> curDirPressed;
    std::map<unsigned, unsigned> curDirReleased;
    std::map<unsigned, unsigned> preDirPressed;
    
    //std::map<int, cc::Point>     cameraID;
    
    
    std::map<unsigned, int>      joyID;
    std::map<unsigned, cc::Vec2> joyPos;
    //selection input
    std::map<unsigned, unsigned> entitySelection;
    ActionMode                   actionSelection;
    
    RoomData *data = nullptr;
    
    std::list<PlayerData*> pList;
};