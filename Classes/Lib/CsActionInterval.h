#pragma once
#include "Headers.h"

class CC_DLL Flicker : public cocos2d::ActionInterval
{
public:
    static Flicker* create(float duration, float interval,
                           const cocos2d::Vec2& opacity,
                           const cocos2d::Vec2& scaleX, const cocos2d::Vec2& scaleY,
                           const cocos2d::Color3B &minColor, const cocos2d::Color3B &maxColor);

    //
    // Overrides
    //
    virtual Flicker* clone() const override;
    virtual Flicker* reverse(void) const  override;
    virtual void startWithTarget(cocos2d::Node *target) override;
    /**
     * @param time in seconds
     */
    virtual void update(float time) override;
    
CC_CONSTRUCTOR_ACCESS:
    Flicker() {}
    virtual ~Flicker() {}
    
    /** initializes the action */
    bool initWithDuration(float duration, float interval,
                          const cocos2d::Vec2& opacity,
                          const cocos2d::Vec2& scaleX,
                          const cocos2d::Vec2& scaleY,
                          const cocos2d::Color3B &minColor,
                          const cocos2d::Color3B &maxColor);
    
protected:
    cocos2d::Vec2 opacity;
    cocos2d::Vec2 scaleX, scaleY;
    cocos2d::Color3B minColor, maxColor;
    double accumulator, lastUpdate;
    float interval;

    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(Flicker);
    
    static const int kMax = 1024;
    static int r[1024];
    int randIndex;
};
