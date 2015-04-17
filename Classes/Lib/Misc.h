#pragma once

#ifdef NDEBUG
#define NOLOG
#endif
#ifdef NOLOG
#define Log(...) do {} while (0)
#else
#define Log(...) cocos2d::log(__VA_ARGS__)
#endif

#include <stdarg.h>
#include <memory>

namespace lib
{
    template<typename T>
    T clamp(const T& val, const T&min, const T& max)
    {
        return (val > max ? max : (val < min ? min : val));
    }
    
    inline float randf01()
    {
        return (float)(rand() % RAND_MAX) / (float)RAND_MAX;
    }
    
    inline float rand()
    {
        return (randf01() - 0.5f) * 2.0f;
    }
    
    inline float randfAB(float a, float b){
        return (rand() / (double)RAND_MAX) * (b - a) + a;
    }
    
    inline unsigned randAB(int a, int b){
        //a included //b excluded
        assert(b > a);
        return std::rand() % (b - a) + a;
    }
    
    inline int parseInt(const std::string& str)
    {
        return (int)strtol(str.c_str(), nullptr, 10);
    }
    
    inline float parseFloat(const std::string& str)
    {
        return strtof(str.c_str(), nullptr);
    }
    
    template<typename T>
    const bool contains( std::vector<T>& v, const T& e)
    {
        return std::find(v.begin(), v.end(), e) != v.end();
    }
    
    template<class T>
    inline bool hasValue(const std::vector<T> v, const T& e)
    {
        return std::find(v.begin(), v.end(), e) != v.end();
    }
    
    template<class T, class U>
    inline bool hasKey(const std::map<T, U>& m, const T& k)
    {
        return m.find(k) != m.end();
    }
    
    template<class U>
    inline bool hasKey(const std::map<std::string, U>& m, const std::string& k)
    {
        return m.find(k) != m.end();
    }
    
    inline std::string lowercase(const std::string &str)
    {
        std::string lower = str;
        transform(lower.begin(),
                  lower.end(),
                  lower.begin(),
                  ::tolower);
        return lower;
    }
    
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
    
    template<class ContainerT>
    void split(const std::string& str,
                  ContainerT& tokens,
                  const std::string& delimiters = " ",
                  bool trimEmpty = false)
    {
        std::string::size_type pos, lastPos = 0;
        
        using value_type = typename ContainerT::value_type;
        using size_type  = typename ContainerT::size_type;
        
        while(true)
        {
            pos = str.find_first_of(delimiters, lastPos);
            if(pos == std::string::npos)
            {
                pos = str.length();
                
                if(pos != lastPos || !trimEmpty)
                    tokens.push_back(value_type(str.data()+lastPos,
                                                (size_type)pos-lastPos ));
                
                break;
            }
            else
            {
                if(pos != lastPos || !trimEmpty)
                    tokens.push_back(value_type(str.data()+lastPos,
                                                (size_type)pos-lastPos ));
            }
            
            lastPos = pos + 1;
        }
    }
    
    bool intersectRects(const std::vector<cocos2d::Rect> &rs1,
                        const std::vector<cocos2d::Rect> &rs2);
    cocos2d::Rect getIntersection(const cocos2d::Rect &r1, const cocos2d::Rect &r2);
    
    double now();
    std::string format(const std::string fmt_str, ...);
    
    template<typename Iter, typename RandomGenerator>
    Iter selectRand(Iter start, Iter end, RandomGenerator& g) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    }
    
    template<typename Iter>
    Iter selectRand(Iter start, Iter end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return selectRand(start, end, gen);
    }
}