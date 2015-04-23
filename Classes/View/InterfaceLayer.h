#pragma once

#include "Defines.h"

class HealthBar;

class InterfaceLayer : public cocos2d::Layer
{
public:
    static InterfaceLayer* create();

    InterfaceLayer();
    
    virtual ~InterfaceLayer();
    
    virtual bool init() override;
    void setTargetID(unsigned eid,
                     bool friendly,
                    cc::Sprite* container,
                    cc::Point pos);
    void unsetTargetID(unsigned eid);
    void clearTarget();
    void setJoystick(cc::Vec2 v);
    void clearJoystick();
    void setAction(ActionMode action);
    ActionMode getAction();
    ActionMode getNextAction();
    ActionMode getPrevAction();
    cc::Rect getActionBounds();
    
    HealthBar* getHealthBar();
    
private:
    cc::ui::Scale9Sprite*   actionSelection;
    cc::Sprite*             actionRun;
    cc::Sprite*             actionSword;
    cc::Sprite*             actionTargetSword;
    
    cc::Sprite* getSpriteAction(ActionMode action);
    
    unsigned curTargetEntityID = 0;
    cc::Sprite* targetEnemy;
    cc::Sprite* targetFriend;
    bool withTarget();
    
    void internalTransitionAction(cc::Sprite* prevSprite, cc::Sprite* nextSprite);
    
    cc::Sprite* cursor;
    cc::Sprite* lArrow;
    cc::Sprite* rArrow;
    cc::Sprite* uArrow;
    cc::Sprite* dArrow;
    
    HealthBar* healthBar;
    
    ActionMode currentAction;
};