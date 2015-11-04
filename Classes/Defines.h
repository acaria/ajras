#pragma once

#define kTraceBehaviours 0

#define ECSYSTEM_DEBUG 1
#define GINFO_DEBUG 1

namespace def {

    //config
    const bool  showFPS = false;
    const float updatesPerSecond = 60.0f;
    const float idealInterval = 1.0f / updatesPerSecond;
    const float ticksPerSecond = /*20*/60;
    const float secondsPerTick = 1.0f / ticksPerSecond;
    const int   blockSize = 16;
    
    //design
    const cc::Rect  canvasMissionRect = {190.0f,15.0f,750.0f,600.0f};
    const cc::Rect  canvasCampRect = {0.0f,0.0f,960.0f,640.0f};
    const float     touchTreshold = 20;
    
    //data
    enum class TriggerMode { ONCE, ALWAYS };
    
    //view
    enum class LayerType : int { BG, MAIN1, MAIN2, FG};
    namespace font
    {
        const std::string mini = "fonts/04b03.ttf";
        const std::string mini2 = "fonts/04b11.ttf";
        const std::string normal = "fonts/romulus.ttf";
    };
    
    //melee
    namespace blinkAnim
    {
        const float duration = 0.5f;
        const int   count = 3;
    }
    
    //AI
    namespace mood
    {
        enum Flags
        {
            None = 0,
            Neutral = 1,
            Hostile = 2,
            Friendly = 4,
            Agressive = 8
        };
        
        inline Flags operator|(Flags a, Flags b)
        { return static_cast<Flags>(static_cast<int>(a) | static_cast<int>(b)); }
        
        inline bool inside(Flags a, Flags  b)
        { return (static_cast<int>(a) & static_cast<int>(b)) != 0; }
        
        static Flags fromStr(const std::string& str)
        {
            if (str == "hostile")
                return Flags::Hostile;
            if (str == "friendly")
                return Flags::Friendly;
            if (str == "agressive")
                return Flags::Agressive;
            return Flags::Neutral;
        }
        
        static Flags getOpponents(Flags a)
        {
            switch(a)
            {
                case Flags::Friendly: return Flags::Hostile | Flags::Agressive;
                case Flags::Hostile : return Flags::Friendly | Flags::Agressive;
                case Flags::Agressive: return Flags::Friendly | Flags::Hostile | Flags::Agressive | Flags::Neutral;
                case Flags::Neutral:
                default: return Flags::None;
            }
        }
        
        static Flags getAllies(Flags a)
        {
            switch(a)
            {
                case Flags::Friendly: return Flags::Friendly;
                case Flags::Hostile : return Flags::Hostile;
                case Flags::Agressive:
                case Flags::Neutral:
                default: return Flags::None;
            }
        }
    }
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