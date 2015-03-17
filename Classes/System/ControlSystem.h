#pragma once
#include "Headers.h"

class GameScene;

class ControlSystem : public BaseTickSystem
{
public:
    //input config
    static constexpr unsigned INDEX_P1 = 1;

    //ctors
    ControlSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {
        this->initControl(INDEX_P1);
    }
    
    virtual ~ControlSystem() {}

    void init(GameScene *gview, MapData* data);
    
    //overrides
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
private:
    void initControl(unsigned index);

    //keyboard
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode code,
                      cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode code,
                       cocos2d::Event* event);
    
    //mouse
    void onMouseMove(cocos2d::Event* event);
    void onMouseUp(cocos2d::Event* event);
    void onMouseDown(cocos2d::Event* event);
    void onMouseScroll(cocos2d::Event* event);
    
    //touch
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchCanceled(cocos2d::Touch* touch, cocos2d::Event* event);
    
    void clearReleased(unsigned controlIndex);
    
    //control input
    std::map<unsigned, unsigned> curDirPressed;
    std::map<unsigned, unsigned> curDirReleased;
    std::map<unsigned, unsigned> preDirPressed;
    std::map<unsigned, unsigned> savedOrientation;
    //selection input
    std::map<unsigned, unsigned> entitySelection;
    
    MapData *data = nullptr;
    GameScene *view = nullptr;
};