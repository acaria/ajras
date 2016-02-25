#pragma once

class FloorData;
class PlayerData;
class NodeRenderer;
class GateMap;

#include "SystemFacade.h"
#include "SystemContext.h"

#include "RoomSystemCtrl.h"
#include "LayeredContainer.h"
#include "Randgine.h"
#include "GameCamera.h"
#include "TransitInfo.h"

class FloorSystemCtrl
{
public:
    FloorSystemCtrl();
    ~FloorSystemCtrl();
    
    void load(GameCamera* cam, cc::Node* view, PlayerData *player, FloorData* data);
    void clear();
    
    //void displayDebug(cc::Node* view, FloorData* data);
    //cc::Sprite* displayMap(FloorData* data);
    
    void tick(double dt);
    void animate(double dt, double tickPercent);
    
    SystemDispatcher& getDispatcher();
    
private:
    //system
    SystemFacade        systemFacade;
    SystemContext       context;
    SystemDispatcher    dispatcher;
    void bindSystems();
    //local ecs
    lib::EcsGroup       ecsGroup;
    
    //loading
    void loadLevel();
    void loadEntities();
    
    //switch management
    void changeEntityRoom(unsigned nextRoomIndex, unsigned eid, const GateMap& gate);
    void moveEntity(unsigned eid, unsigned prevRoomIndex, unsigned nextRoomIndex);
    void switchRoom(unsigned fromRoomIndex, unsigned toRoomIndex, cc::Vec2 destPos,
            const std::function<void()> after = nullptr);
    void showEntityFromGate(unsigned roomIndex, unsigned eid, const GateMap& gate, float duration,
            const std::function<void()>& after = nullptr);
    void regroupTeam(unsigned eid, unsigned nextRoomIndex, const GateMap& gate,
                     const std::function<void()>& onReady);
    
    void showRoom(unsigned roomIndex, std::function<void()> after = nullptr);
    bool isInTransit(unsigned eid);
    
    //data
    FloorData*                              data;
    std::map<unsigned, LayeredContainer*>   roomViews;
    std::map<unsigned, NodeRenderer*>       roomPreviews;
    std::map<unsigned, RoomSystemCtrl*>     roomSystems;
    
    //view
    cc::Node*   view = nullptr;
    GameCamera* cam = nullptr;
    
    //event
    std::vector<lib::Registration> eventRegs;
    
    PlayerData* playerData = nullptr;
    
    //internal
    TransitInfo transitInfo;
};
