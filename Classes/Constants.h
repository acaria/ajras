#pragma once

#define kDrawDebug 0
#define kDrawInfo 0

#define kTraceBehaviours 0

namespace cc = cocos2d;

const bool  kShowFPS = true;
const float kUpdatesPerSecond = 60.0f;
const float kIdealInterval = 1.0f / kUpdatesPerSecond;
const float kTicksPerSecond = 20;
const float kSecondsPerTick = 1.0f / kTicksPerSecond;

const int   kBlockSize = 16;

const cc::Rect  kCanvasRect = {190.0f,15.0f,750.0f,600.0f};
const cc::Point kCursorCenter = {90.0f,90.0f};

enum CollisionCategory
{
    impassable,
    walkable,
    flyable,
};
