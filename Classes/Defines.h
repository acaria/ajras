#pragma once

#define kDrawDebug 0
#define kDrawInfo 0

#define kTraceBehaviours 0

#define ECSYSTEM_DEBUG 0

namespace def {
    //config
    const bool showFPS = true;
    const float updatesPerSecond = 60.0f;
    const float idealInterval = 1.0f / updatesPerSecond;
    const float ticksPerSecond = 20;
    const float secondsPerTick = 1.0f / ticksPerSecond;
    const int   blockSize = 16;
    
    //design
    const cc::Rect  canvasMissionRect = {190.0f,15.0f,750.0f,600.0f};
    const cc::Rect  canvasCampRect = {0.0f,0.0f,960.0f,640.0f};
    const float     touchTreshold = 20;
}

static cocos2d::Size designResolutionSize = cocos2d::Size(960, 640);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

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
