#pragma once

namespace cc = cocos2d;

inline cc::Vec2 patchPos(cc::Vec2 pos)
{
    return cc::Vec2(
        floor(pos.x) + 0.004,
        floor(pos.y) + 0.004);
}