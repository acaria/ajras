#pragma once

#define kTraceBehaviours 0

#define ECSYSTEM_DEBUG 1
#define HEADER_DEBUG 1
#define FOOTER_DEBUG 1

namespace def {

    //app
    const cc::Size designResolutionSize = {1024, 768};
    const cc::Size sdResolutionSize = {480, 320};
    const cc::Size hdResolutionSize = {1024, 768};

    //config
    const bool  antialias = false;
    const bool  showFPS = false;
    const float updatesPerSecond = 60.0f;
    const float idealInterval = 1.0f / updatesPerSecond;
    const float ticksPerSecond = 60;
    const float secondsPerTick = 1.0f / ticksPerSecond;
    
    //design
    //margin x:left, y:right, z:up, w:down
    const cc::Vec4  canvasCampMargin = {0,0,0,0};
    const cc::Vec4  canvasMissionMargin = {190.0f,30.0f,30.0f,30.0f};
    
    const cc::Color4B     bgColor = {143,86,59,255};
    const float     touchTreshold = 20;
    
    //data
    enum class TriggerMode { ONCE, ALWAYS };
    struct SleepZone
    {
        cc::Rect    bounds;
        bool        taken;
    };
    
    //view
    enum class LayerType : int { BG, MAIN1, MAIN2, FG};
    namespace font
    {
        const std::string mini = "fonts/04b03.ttf";
        const std::string mini2 = "fonts/04b11.ttf";
        const std::string normal = "fonts/romulus.ttf";
    };
    
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
        
        inline Flags fromStr(const std::string& str)
        {
            if (str == "hostile")
                return Flags::Hostile;
            if (str == "friendly")
                return Flags::Friendly;
            if (str == "agressive")
                return Flags::Agressive;
            return Flags::Neutral;
        }
        
        inline Flags getOpponents(Flags a)
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
        
        inline Flags getAllies(Flags a)
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
    
    namespace collision
    {
        enum class Cat
        {
            impassable,
            walkable,
            flyable,
            collectible
        };
        
        struct Agent
        {
            unsigned    id;
            cc::Rect    bounds;
            cc::Rect    lastBounds;
            Cat         category;
            cc::Vec2    velocity;
        };
    }
}

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