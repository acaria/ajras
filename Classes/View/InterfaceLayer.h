#pragma once
#include "Headers.h"

class InterfaceLayer : public cocos2d::Layer
{
public:
    static InterfaceLayer* create();

    InterfaceLayer();
    
    virtual ~InterfaceLayer();
    
    virtual bool init() override;
    void setTargetID(unsigned eid, bool friendly);
    void clearTarget();
    
private:
    unsigned curTargetEntityID = 0;
    cocos2d::Sprite* targetEnemy;
    cocos2d::Sprite* targetFriend;
};