#pragma once

#define kDrawDebug 0
#define kDrawInfo 0

#define kTraceBehaviours 0

const bool  kShowFPS = true;
const float kUpdatesPerSecond = 60.0f;
const float kIdealInterval = 1.0f / kUpdatesPerSecond;
const float kTicksPerSecond = 20;
const float kSecondsPerTick = 1.0f / kTicksPerSecond;

const int   kBlockSize = 16;

static cocos2d::Size designResolutionSize = cocos2d::Size(960, 640);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

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
    team,
    map,
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
