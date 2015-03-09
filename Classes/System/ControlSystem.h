#pragma once
#include "Headers.h"

class ControlSystem : public BaseTickSystem
{
public:
    void init(cocos2d::Node *view);
    
    ControlSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    virtual ~ControlSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
private:
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode code,
                      cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode code,
                       cocos2d::Event* event);
    void clearReleased();
    
    unsigned curDirPressed;
    unsigned curDirReleased;
    unsigned preDirPressed;
    unsigned savedOrientation;
};