#pragma once

#define kDrawDebug 0

const bool  kShowFPS = true;
const float kUpdatesPerSecond = 60.0f;
const float kIdealInterval = 1.0f / kUpdatesPerSecond;
const float kTicksPerSecond = 25;
const float kSecondsPerTick = 1.0f / kTicksPerSecond;

enum CollisionCategory
{
    impassable,
    walkable,
    flyable,
};

enum Dir
{
    kNone = 0,
    kLeft = 1,
    kRight = 2,
    kUp = 4,
    kDown = 8
};