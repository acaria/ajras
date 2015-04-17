#include "CsActionInterval.h"
#include "Misc.h"

Flicker* Flicker::create(float duration, float interval,
                         const cocos2d::Vec2& opacity,
                         const cocos2d::Vec2& scaleX, const cocos2d::Vec2& scaleY,
                         const cocos2d::Color3B &minColor, const cocos2d::Color3B &maxColor)
{
    Flicker *ret = new (std::nothrow) Flicker();
    
    if (ret)
    {
        if (ret->initWithDuration(duration, interval, opacity, scaleX, scaleY, minColor, maxColor))
        {
            ret->autorelease();
        }
        else
        {
            delete ret;
            ret = nullptr;
        }
    }
    
    return ret;
}

bool Flicker::initWithDuration(float duration, float interval,
                               const cocos2d::Vec2& opacity,
                               const cocos2d::Vec2& scaleX,
                               const cocos2d::Vec2& scaleY,
                               const cocos2d::Color3B &minColor,
                               const cocos2d::Color3B &maxColor)
{
    randIndex = 0;
    if (r[0] == 0)
    {
        for(int i = 0; i < kMax; ++i)
            r[i] = cocos2d::random();
    }
    this->accumulator = 0.0;
    this->lastUpdate = 0.0;
    
    bool ret = false;
    
    if (ActionInterval::initWithDuration(duration))
    {
        this->interval = interval;
        this->opacity = opacity;
        this->scaleX = scaleX;
        this->scaleY = scaleY;
        this->minColor = minColor;
        this->maxColor = maxColor;
        ret = true;
    }
    
    return ret;
}

Flicker* Flicker::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) Flicker();
    a->initWithDuration(_duration, interval, opacity, scaleX, scaleY, minColor, maxColor);
    a->autorelease();
    return a;
}

int Flicker::r[] = {0};

void Flicker::startWithTarget(cc::Node *target)
{
    ActionInterval::startWithTarget(target);
}

Flicker* Flicker::reverse() const
{
    return Flicker::create(_duration, interval, opacity, scaleX, scaleY, minColor, maxColor);
}


void Flicker::update(float t)
{
    double currentTime = lib::now();
    if (lastUpdate <= 0)
        lastUpdate = currentTime;
    accumulator += currentTime - lastUpdate;
    lastUpdate = currentTime;
    
    if (_target && accumulator > interval)
    {
        accumulator -= interval;
        if (opacity.y - opacity.x > 0)
            _target->setOpacity(opacity.x + (r[randIndex+0] % (int)(opacity.y - opacity.x)));
        if (scaleX.y - scaleX.x > 0)
            _target->setScaleX(scaleX.x + ((r[randIndex+1] / (float)RAND_MAX) * (scaleX.y - scaleX.x)));
        if (scaleY.y - scaleY.x > 0)
            _target->setScaleY(scaleY.x + ((r[randIndex+2] / (float)RAND_MAX) * (scaleY.y - scaleY.x)));
        
        cc::Color3B color;
        float alpha = (float)r[randIndex+3] / (float)RAND_MAX;
        color.r = ((float)minColor.r * alpha) + ((float)maxColor.r * (1.0f - alpha));
        color.g = ((float)minColor.g * alpha) + ((float)maxColor.g * (1.0f - alpha));
        color.b = ((float)minColor.b * alpha) + ((float)maxColor.b * (1.0f - alpha));
        _target->setColor(color);
    }
    
    randIndex = (randIndex + 4) % kMax;
}
