#pragma once

#include "Event.h"

class StickControl : public cc::Node
{
public:
    static StickControl* create(const std::string& bg,
                                const std::string& stick,
                                int area, int range);
    
    StickControl(int area, int range):  range(range),
                                        area(area) {};
    
    lib::Subject<void(cc::Vec2 dir)> onTrigger;
    lib::Subject<void()>             onRelease;
    
private:
    bool initWithKey(const std::string& bg,
                     const std::string& stick);
    void setStickPosition(cc::Point position);
    
    cc::Sprite* bg;
    cc::Sprite* stick;
    int range;
    int area;
};