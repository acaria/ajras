#pragma once

struct PositionComponent
{
    PositionComponent() {
        pos = {0, 0};
        last = {0, 0};
    }
    
    void set(const cc::Vec2 &pos)
    {
        this->pos = pos;
        this->last = pos;
    }
    
    cc::Vec2 pos;
    cc::Vec2 last;
};