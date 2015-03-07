#pragma once
#include "Headers.h"

class InputSystem : public BaseTickSystem
{
public:
    void init(cocos2d::Node *view);
    
    InputSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    ~InputSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final;
    
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