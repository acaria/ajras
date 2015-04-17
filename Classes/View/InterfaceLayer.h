#pragma once

class InterfaceLayer : public cocos2d::Layer
{
public:
    static InterfaceLayer* create();

    InterfaceLayer();
    
    virtual ~InterfaceLayer();
    
    virtual bool init() override;
    void setTargetID(unsigned eid, bool friendly);
    void clearTarget();
    void setJoystick(cc::Vec2 v);
    void clearJoystick();
    
private:
    unsigned curTargetEntityID = 0;
    cc::Sprite* targetEnemy;
    cc::Sprite* targetFriend;
    
    cc::Sprite* cursor;
    cc::Sprite* lArrow;
    cc::Sprite* rArrow;
    cc::Sprite* uArrow;
    cc::Sprite* dArrow;
};