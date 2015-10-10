#pragma once

#include "Defines.h"

class HealthBar;
class InventoryPanel;

class MissionInterfaceLayer : public cc::Layer
{
public:
 
    static MissionInterfaceLayer* create();

    MissionInterfaceLayer();
    virtual ~MissionInterfaceLayer();
    
    virtual bool init() override;
    void        setTargetID(unsigned eid,
                    bool friendly,
                    cc::Sprite* container,
                    cc::Point pos);
    void        unsetTargetID(unsigned eid);
    void        clearTarget();
    cc::Vec2    setJoystick(cc::Point pos);
    void        clearJoystick();
    void        setActionMode(ActionMode action);
    void        setActionPanel(ActionMode action);
    ActionMode  getAction();
    ActionMode  getNextAction();
    ActionMode  getPrevAction();
    cc::Rect    getActionBounds();
    
    //accessors
    HealthBar*      getHealthBar();
    InventoryPanel* getInventoryPanel();
    
private:
    //consts
    const cc::Point kCursorCenter = {90.0f,90.0f};
    const cc::Point kCursorRegion = {30.0f,30.0f};

    cc::ui::Scale9Sprite*   actionSelection;
    
    cc::Sprite*                     actionTeam;
    cc::Sprite*                     actionTeamHi;
    std::pair<cc::Point, cc::Point> actionTeamPos;
    
    cc::Sprite*                     actionMap;
    cc::Sprite*                     actionMapHi;
    std::pair<cc::Point, cc::Point> actionMapPos;
    
    cc::Sprite*                     actionInventorize;
    cc::Sprite*                     actionInventorizeHi;
    std::pair<cc::Point, cc::Point> actionInventorizePos;
    
    //cc::Sprite*             actionTargetSword;
    
    InventoryPanel*                 inventoryPanel;
    
    unsigned curTargetEntityID = 0;
    cc::Sprite* targetEnemy;
    cc::Sprite* targetFriend;
    bool withTarget();
    
    cc::Sprite* joyStick;
    cc::Sprite* joyBase;
    
    HealthBar* healthBar;
    
    ActionMode currentAction;
};