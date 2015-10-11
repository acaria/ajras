#pragma once
class GameCamera
{
private:
    const int CAMERA_ID = 1;
    const float MIN_SCALE = 0.8;
    const float MAX_SCALE = 1.8;
    const float FOCUS_MARGIN = 50.0;
    
public:
    GameCamera(cc::Node* playground, cc::Rect bounds);
    
    void setInnerArea(cc::Rect);
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

private:

    //cam content
    cc::Node*   playground;
    
    //cam params
    cc::Point   centerPos;
    cc::Point   curPosition;
    float       curScale = 1.0f;
    cc::Size    groundSize;
    DataFocus   focus;
    cc::Rect    canvasRect;
    cc::Rect    innerAreaRect;
    
    //internal
    void updatePos();
    bool moving = false;
    std::map<int, cc::Point> cameraID;
    cc::Rect computeRect(cc::Point p1, cc::Point p2);
};

