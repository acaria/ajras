#pragma once
#include "Event.h"

class GameCamera
{
private:
    const int CAMERA_ID = 1;
    const float MIN_SCALE = 0.8;
    const float MAX_SCALE = 1.8;
    const float FOCUS_MARGIN = 50.0;
    
public:
    GameCamera(cc::Node* playground, cc::Rect canvasRect);
    
    void setFrameBounds(cc::Rect bounds);
    void setTarget(cc::Point pos);
    void translate(cc::Point translation);
    void setScale(float scale);
    void addScale(float scale);
    void moveTarget(cocos2d::Vec2 pos, float duration);
    void focusTarget(cc::Point pos);
    void unfocus();

    struct DataFocus
    {
        bool        enabled = false;
        cc::Point   target;
    };

    //events
    lib::Subject<void(cc::Point)>               onTouch;
    lib::Subject<void(cc::Point, cc::Point)>    onSwipe;

private:

    //cam content
    cc::Node*   playground;
    
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

