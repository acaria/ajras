#pragma once
#include "Defines.h"
#include "Randgine.h"
#include "ECSGroup.h"
#include "SystemDispatcher.h"

class RoomData;
class LayeredContainer;
class MissionScene;
class GateMap;

class RoomSystemCtrl
{
public:
    RoomSystemCtrl(unsigned group,
                   LayeredContainer* view,
                   RoomData* data,
                   SystemDispatcher& dispatcher);
    
    RoomData* changeRoom(unsigned roomIndex, unsigned gateIndex, const std::vector<unsigned>& eids);

    void tick(double dt);
    void animate(double dt, double tickPercent);

    //dispays
    void hideObjects(float duration);
    void showObjects(float duration);

private:
    void loadZoneObject(const std::string &profileName,
                        const cc::Rect& bounds,
                        RoomData *data,
                        LayeredContainer *view);
    
    lib::Random&        random;
    SystemDispatcher&   dispatcher;
    lib::EcsGroup       ecsGroup;
};