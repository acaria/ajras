#pragma once

#define kDrawDebug 0
#define kDrawInfo 0

#define kTraceBehaviours 1

const bool  kShowFPS = true;
const float kUpdatesPerSecond = 60.0f;
const float kIdealInterval = 1.0f / kUpdatesPerSecond;
const float kTicksPerSecond = 20;
const float kSecondsPerTick = 1.0f / kTicksPerSecond;

const int   kBlockSize = 16;

const cocos2d::Rect  kCanvasRect = {190.0f,15.0f,750.0f,600.0f};

enum CollisionCategory
{
    impassable,
    walkable,
    flyable,
};

enum class ActionMode
{
    none,
    explore,
    attack,
    inventorize
};