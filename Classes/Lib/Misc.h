#pragma once

#include <stdarg.h>
#include <memory>

namespace lib
{
    template<typename T>
    T clamp(const T& value, const T& min, const T& max) {
        return min < max ?
            (value < min ? min : value > max ? max : value) :
            (value < max ? max : value > min ? min : value);
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
    
    bool intersectRects(const std::vector<cocos2d::Rect> &rs1,
                        const std::vector<cocos2d::Rect> &rs2);
    cc::Rect getIntersection(const cocos2d::Rect &r1, const cocos2d::Rect &r2);
    cc::Rect getUnion(const cc::Rect &r1, const cc::Rect& r2);
    cc::Rect inflateRect(const cc::Rect &r, float value);
    
    double now();

    std::string format(const std::string fmt_str, ...);
    
    constexpr unsigned int hash(const char* str, int h = 0)
    {
        return !str[h] ? 5381 : (hash(str, h+1)*33) ^ str[h];
    }
    
    constexpr unsigned int hash(const std::string& str, int h = 0)
    {
        return hash(str.c_str(), h);
    }
}