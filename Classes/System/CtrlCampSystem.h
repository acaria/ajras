#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"
#include "CampInterfaceLayer.h"

class CampScene;
class CampData;

class CtrlCampSystem : public BaseTickSystem
{
public:
    //ctors
    CtrlCampSystem(lib::EcsGroup& ecs);
    virtual ~CtrlCampSystem() {}
    
    void init(CampScene *gview, CampData* data);
    
    //overrides
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
private:
    void initControl(unsigned index);
    
    
    void clearReleased(unsigned controlIndex);
    
    //internal methods
    static cc::Rect computeRect(cc::Point p1, cc::Point p2);
    
    //control input
    std::map<unsigned, unsigned> curDirPressed;
    std::map<unsigned, unsigned> curDirReleased;
    std::map<unsigned, unsigned> preDirPressed;
    
    std::map<int, cc::Point>     cameraID;
    
    
    std::map<unsigned, int>      joyID;
    std::map<unsigned, cc::Vec2> joyPos;
    
    CampData *data = nullptr;
    CampScene *view = nullptr;
    
    std::list<PlayerData*> pList;
};