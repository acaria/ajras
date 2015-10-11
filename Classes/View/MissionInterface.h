#pragma once

#include "Defines.h"
#include "StickControl.h"
#include "HealthBar.h"
#include "InventoryPanel.h"
#include "Event.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

class MissionInterface : public cc::Node
{
public:
 
    static MissionInterface* create();

    MissionInterface();
    virtual ~MissionInterface();
    
    virtual bool init() override;
    
    void registerPlayer(unsigned playerIndex,
            std::function<CtrlKeyType(KeyCode)> onKeyCode2KeyType);
    
    void        setTargetID(unsigned eid,
                    bool friendly,
                    cc::Sprite* container,
                    cc::Point pos);
    void        unsetTargetID(unsigned eid);
    void        clearTarget();
    
    ActionMode  getAction();
    ActionMode  getNextAction();
    ActionMode  getPrevAction();
    cc::Rect    getActionBounds();
    
    //accessors
    HealthBar*      getHealthBar();
    InventoryPanel* getInventoryPanel();
    StickControl*   getStick();
    
    //events
    lib::Subject<void(unsigned, int)> onKeyPressAction;
    lib::Subject<void(unsigned, int)> onKeyReleaseAction;
    lib::Subject<void(ActionMode)>    onSetActionMode;
    
private:
    std::function<CtrlKeyType(KeyCode)> onKeyCode2KeyType = nullptr;
    
    cc::ui::Scale9Sprite*           actionSelection;
    
    cc::Sprite*                     actionTeam;
    cc::Sprite*                     actionTeamHi;
    std::pair<cc::Point, cc::Point> actionTeamPos;
    
    cc::Sprite*                     actionMap;
    cc::Sprite*                     actionMapHi;
    std::pair<cc::Point, cc::Point> actionMapPos;
    
    cc::Sprite*                     actionInventorize;
    cc::Sprite*                     actionInventorizeHi;
    std::pair<cc::Point, cc::Point> actionInventorizePos;
    
    InventoryPanel*                 inventoryPanel;
    
    void                            setActionMode(ActionMode action);
    void                            setActionPanel(ActionMode action);
    
    unsigned curTargetEntityID = 0;
    cc::Sprite* targetEnemy;
    cc::Sprite* targetFriend;
    bool withTarget();
    
    StickControl* stick;
    HealthBar* healthBar;
    
    ActionMode currentAction;
    
    unsigned playerIndex = 0;
};