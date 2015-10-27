#include "Misc.h"

namespace lib
{

    double now()
    {
        return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    bool intersectRects(const std::vector<cc::Rect> &rs1,
                        const std::vector<cc::Rect> &rs2)
    {
        for(auto r1 : rs1)
        for(auto r2 : rs2)
        {
            if (r1.intersectsRect(r2))
                return true;
        }
        
        return false;
    }
    
    cc::Rect getIntersection(const cc::Rect &r1, const cc::Rect &r2)
    {
        cocos2d::Vec2 origin = {
            MAX(r1.getMinX(), r2.getMinX()),
            MAX(r1.getMinY(), r2.getMinY())
        };
        
        return {
            origin.x,
            origin.y,
            MIN(r1.getMaxX(), r2.getMaxX()) - origin.x,
            MIN(r1.getMaxY(), r2.getMaxY()) - origin.y
        };
    }
    
    cc::Rect getUnion(const cc::Rect &r1, const cc::Rect& r2)
    {
        cocos2d::Vec2 origin = {
            MIN(r1.getMinX(), r2.getMinX()),
            MIN(r1.getMinY(), r2.getMinY())
        };
        
        return {
            origin.x,
            origin.y,
            MAX(r1.getMaxX(), r2.getMaxX()) - origin.x,
            MAX(r1.getMaxY(), r2.getMaxY()) - origin.y
        };
    }
    
    cc::Rect inflateRect(const cc::Rect &r, float value)
    {
        float inflateValue = value / 2;
        return {
            r.origin.x - inflateValue,
            r.origin.y - inflateValue,
            r.size.width + value,
            r.size.height + value
        };
    }
    
    std::string format(const std::string fmt_str, ...)
    {
        int final_n, n = ((int)fmt_str.size()) * 2;
        std::string str;
        std::unique_ptr<char[]> formatted;
        va_list ap;
        while(1) {
            formatted.reset(new char[n]);
            strcpy(&formatted[0], fmt_str.c_str());
            va_start(ap, fmt_str);
            final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
            va_end(ap);
            if (final_n < 0 || final_n >= n)
                n += abs(final_n - n + 1);
            else
                break;
        }
        return std::string(formatted.get());
    }
}
