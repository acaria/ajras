#pragma once

#define kDrawDebug 1
#define kDrawInfo 1

#define kTraceBehaviours 0

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
    collectible
};

enum class ActionMode
{
    none,
    explore,
    attack,
    inventorize
};

enum class CtrlKeyType
{
    none,
    left, right, up, down,
    autoselect,
    sel1, sel2, sel3
};

enum class ColCat
{
    //bag
    object,
    currency,
    //equipemnt
    head,
    body,
    arm,
    leg,
    feet,
    ring
};
