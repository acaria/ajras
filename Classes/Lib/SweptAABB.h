#pragma once

#include <cmath>
#include <algorithm>
#include <limits>

namespace lib
{
    // describes an axis-aligned rectangle with a velocity
    struct Box
    {
        Box(const cocos2d::Rect& rect,
            const cocos2d::Vec2& velocity) : Box(rect.origin.x,
                                                 rect.origin.y,
                                                 rect.size.width,
                                                 rect.size.height,
                                                 velocity.x, velocity.y) {}
        
        Box(float _x, float _y, float _w, float _h, float _vx, float _vy)
        {
            x = _x;
            y = _y;
            w = _w;
            h = _h;
            vx = _vx;
            vy = _vy;
        }
        
        Box(float _x, float _y, float _w, float _h)
        {
            x = _x;
            y = _y;
            w = _w;
            h = _h;
            vx = 0.0f;
            vy = 0.0f;
        }
        
        cocos2d::Rect getDest()
        {
            return cocos2d::Rect(x + vx, y + vy, w, h);
        }
        
        // position of top-left corner
        float x, y;
        
        // dimensions
        float w, h;
        
        // velocity
        float vx, vy;
    };
    
    // returns true if the boxes are colliding (velocities are not used)
    bool AABBCheck(Box b1, Box b2);
    
    // returns true if the boxes are colliding (velocities are not used)
    // moveX and moveY will return the movement the b1 must move to avoid the collision
    bool AABB(Box b1, Box b2, float& moveX, float& moveY);
    
    // returns a box the spans both a current box and the destination box
    Box GetSweptBroadphaseBox(Box b);
    
    // performs collision detection on moving box b1 and static box b2
    // returns the time that the collision occured (where 0 is the start of the movement and 1 is the destination)
    // getting the new position can be retrieved by box.x = box.x + box.vx * collisiontime
    // normalx and normaly return the normal of the collided surface (this can be used to do a response)
    float SweptAABB(Box b1, Box b2, float& normalx, float& normaly);
}