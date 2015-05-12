#pragma once
class GameCamera
{
public:
    GameCamera(cc::Layer* playground, cc::Rect bounds);
    
    void setTarget(cc::Point pos);
    void translate(cc::Point translation);
    void setScale(float scale);
    void addScale(float scale);
    float getScale();
    cc::Point getOrigin() const;
    void moveTarget(cocos2d::Vec2 pos, float duration);
    void focusTarget(cc::Point pos);
    void unfocus();

    struct DataFocus
    {
        bool        enabled = false;
        cc::Point   target;
    };

private:
    cc::Layer*  playground;
    cc::Point   centerPos;
    cc::Point   curPosition;
    float       curScale = 1.0f;
    cc::Size    groundSize;
    DataFocus   focus;
    
    const int CAMERA_ID = 1;
    const float MIN_SCALE = 0.8;
    const float MAX_SCALE = 1.8;
    const float FOCUS_MARGIN = 50.0;
    
    void updatePos();
    
    bool moving;
};

