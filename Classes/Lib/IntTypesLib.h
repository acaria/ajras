#pragma once

namespace lib
{
    struct IntTime
    {
        typedef long IntType;
        typedef double FloatType;
        
        static const int kMultiplier = 1000;
        static constexpr float kDivisor = 1.0f / (float)kMultiplier;
        
        IntType t;
        
        IntTime() : t(0) {}
        IntTime(IntType t) : t(t) {}
        IntTime(const IntTime& rhs):t(rhs.t) {}
        IntTime& operator=(const IntTime& rhs) {t = rhs.t; return *this; }
        
        IntTime operator+(const IntTime& rhs) { return IntTime(t + rhs.t); }
        IntTime& operator+=(const IntTime& rhs) { t += rhs.t; return *this; }
        
        IntTime operator-(const IntTime& rhs) { return IntTime(t - rhs.t); }
        IntTime& operator-=(const IntTime& rhs) { t -= rhs.t; return *this; }
        
        IntTime operator*(const IntTime& rhs) { return IntTime(t * rhs.t); }
        IntTime& operator*=(const IntTime& rhs) { t *= rhs.t; return *this; }
        
        IntTime operator/(const IntTime& rhs) { return IntTime(t / rhs.t); }
        IntTime& operator/=(const IntTime& rhs) { t /= rhs.t; return *this; }
        
        IntTime(FloatType t) : t(t * kMultiplier) {}
        IntTime& operator=(const double rhs) {t= rhs * kMultiplier; return *this; }
        double toDouble() { return (double)t * kDivisor; }
        
        bool operator==(double rhs) { return t == (IntType)(rhs * kMultiplier); }
        bool operator!=(double rhs) { return t != (IntType)(rhs * kMultiplier); }
        bool operator<(double rhs) { return t < (IntType)(rhs * kMultiplier); }
        bool operator>(double rhs) { return t > (IntType)(rhs * kMultiplier); }
        
        bool operator==(const IntTime& rhs) { return t == rhs.t; }
        bool operator!=(const IntTime& rhs) { return t != rhs.t; }
        bool operator<(const IntTime& rhs) { return t < rhs.t; }
        bool operator>(const IntTime& rhs) { return t > rhs.t; }
    };
    
    struct IntVec2
    {
        static const int kMultiplier = 1000;
        static constexpr float kDivisor = 1.0f / (float)kMultiplier;
        
        int x, y;
        
        IntVec2() : x(0), y(0) {}
        IntVec2(int v) : x(v), y(v) {}
        IntVec2(int x, int y) : x(x), y(y) {}
        IntVec2& operator=(const IntVec2& rhs) { x = rhs.x; y = rhs.y; return *this;}
        IntVec2 operator+(const IntVec2& rhs) {return IntVec2(x + rhs.x, y + rhs.y);}
        IntVec2& operator+=(const IntVec2& rhs) {x += rhs.x, y += rhs.y; return *this;}
        IntVec2 operator-(const IntVec2& rhs) {return IntVec2(x - rhs.x, y - rhs.y);}
        IntVec2& operator-=(const IntVec2& rhs) {x -= rhs.x, y -= rhs.y; return *this;}
        IntVec2 operator*(const IntVec2& rhs) {return IntVec2(x * rhs.x, y * rhs.y);}
        IntVec2& operator*=(const IntVec2& rhs) {x *= rhs.x, y *= rhs.y; return *this;}
        IntVec2 operator/(const IntVec2& rhs) {return IntVec2(x / rhs.x, y / rhs.y);}
        IntVec2& operator/=(const IntVec2& rhs) {x /= rhs.x, y /= rhs.y; return *this;}
        IntVec2(float rhs):x(rhs*kMultiplier), y(rhs*kMultiplier) {}
        IntVec2(const cocos2d::Vec2& rhs):x(rhs.x*kMultiplier), y(rhs.y*kMultiplier) {}
        IntVec2& operator=(const cocos2d::Vec2& rhs) {x = rhs.x*kMultiplier; y = rhs.y*kMultiplier; return *this;}
        cocos2d::Vec2 toVec2() {return cocos2d::Vec2((float)x * kDivisor, (float)y * kDivisor);}
        float getVec2X() {return (float)x * kDivisor;}
        float getVec2Y() {return (float)y * kDivisor;}
    };
};