#pragma once
#include "Event.h"
#include "ViewNode.h"
#include "Defines.h"

class GameCamera
{
public:
    GameCamera(ViewNode* playground, cc::Rect canvasRect, const def::camera::Config& config);
    
    void setFrameBounds(cc::Rect bounds);
    void setTargetPos(cc::Point pos);
    void translate(cc::Point translation);
    void setScale(float scale);
    void setTargetSize(cc::Size size);
    void addScale(float scale);
    void moveTarget(cocos2d::Vec2 pos, float duration);
    void moveTarget(cocos2d::Vec2 targetPos, cc::Size targetSize, float duration);
    void focusTarget(cc::Point pos);

    struct DataFocus
    {
        bool        enabled = false;
        cc::Point   target;
    };

    //events
    lib::Subject<void(cc::Point)>               onTouch;
    lib::Subject<void(cc::Point, cc::Point)>    onSwipe;

private:

    def::camera::Config config;

    //cam content
    ViewNode*   playground;
    
    //cam params
    cc::Point   centerPos;
    cc::Point   curPosition;
    float       curScale = 1.0f;
    DataFocus   focus;
    cc::Rect    canvasRect;
    cc::Rect    frameRect;
    
    //internal
    void updatePos();
    bool moving = false;
    std::map<int, cc::Point> cameraID;
    cc::Rect computeRect(cc::Point p1, cc::Point p2);
    cc::Vec2 prevBRMouseLocation;
};

