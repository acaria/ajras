#pragma once

#include "Event.h"

class StickControl : public cc::Node
{
public:
    lib::Subject<void(cc::Vec2 dir)> onTrigger;
    lib::Subject<void()>             onRelease;
    
    bool init(const std::string& bg,
              const std::string& stick,
              int area, int range);
    
private:
    void setStickPosition(cc::Point position);
    
    cc::Sprite* bg;
    cc::Sprite* stick;
    int range;
    int area;
};