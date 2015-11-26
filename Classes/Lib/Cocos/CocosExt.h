#pragma once

namespace cocos2d
{
    inline const Rect operator+(const Rect &rect, const Vec2& point)
    {
        return Rect(rect.origin + point, rect.size);
    }
    
    inline const Rect operator+(const Vec2& point, const Rect &rect)
    {
        return Rect(rect.origin + point, rect.size);
    }
    
    inline const Rect operator-(const Rect &rect, const Vec2& point)
    {
        return Rect(rect.origin - point, rect.size);
    }
    
    inline const Rect operator-(const Vec2& point, const Rect &rect)
    {
        return Rect(rect.origin - point, rect.size);
    }
}