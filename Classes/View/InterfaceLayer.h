#pragma once

#include "Defines.h"

class HealthBar;
class InventoryPanel;

class InterfaceLayer : public cc::Layer
{
public:
 
    static InterfaceLayer* create();

    InterfaceLayer();
    
    virtual ~InterfaceLayer();
    
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
    
    HealthBar*  getHealthBar();
    
private:
    //consts
    const cc::Point kCursorCenter = {90.0f,90.0f};
    const cc::Point kCursorRegion = {40.0f,40.0f};

    cc::ui::Scale9Sprite*   actionSelection;
    
    cc::Sprite*                     actionExplore;
    cc::Sprite*                     actionExploreHi;
    std::pair<cc::Point, cc::Point> actionExplorePos;
    
    cc::Sprite*                     actionAttack;
    cc::Sprite*                     actionAttackHi;
    std::pair<cc::Point, cc::Point> actionAttackPos;
    
    cc::Sprite*                     actionInventorize;
    cc::Sprite*                     actionInventorizeHi;
    std::pair<cc::Point, cc::Point> actionInventorizePos;
    
    //cc::Sprite*             actionTargetSword;
    
    InventoryPanel*                 inventoryPanel;
    
    unsigned curTargetEntityID = 0;
    cc::Sprite* targetEnemy;
    cc::Sprite* targetFriend;
    bool withTarget();
    
    cc::Sprite* cursor;
    cc::Sprite* lArrow;
    cc::Sprite* rArrow;
    cc::Sprite* uArrow;
    cc::Sprite* dArrow;
    
    HealthBar* healthBar;
    
    ActionMode currentAction;
};