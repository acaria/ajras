#pragma once


#define ECSYSTEM_TRACE 1
#define BEHAVIOURS_TRACE 0

#define ECSYSTEM_DEBUG 1
#define HEADER_DEBUG 1
#define FOOTER_DEBUG 1

namespace def {

    //control indexes
    const unsigned CTRL1 = 1;
    const unsigned CTRL2 = 2;
    
    //team ID
    const unsigned PTEAM = 1;
    
    //group IDS
    const unsigned startGroupIndex = 1;
    const unsigned startEntityIndex = 1;
    
    //debug IDS
    const unsigned debugTagDrawID = 1;

    //app
    const cc::Size designResolutionSize = {1024, 768};
    
    typedef struct {
        cc::Size select;
        cc::Size size;
        char directory[100];
    } Resource;
    
    const std::list<Resource> resourceList = {
        {{0,0},         {1024, 768},     "hd"},
    };

    //config
    const bool  antialias = false;
    const bool  showFPS = false;
    const bool  postProcessing = true;
    const float updatesPerSecond = 60.0f;
    const float idealInterval = 1.0f / updatesPerSecond;
    const float ticksPerSecond = 20;
    const float secondsPerTick = 1.0f / ticksPerSecond;
    
    //design
    //margin x:left, y:right, z:up, w:down
    const cc::Vec4      canvasCampMargin = {0,0,0,0};
    const cc::Vec4      canvasMissionMargin = {190.0f,30.0f,30.0f,30.0f};
    
    const cc::Color4B   bgColor = {143,86,59,255};
    const float         touchTreshold = 20;
    
    //data
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
        const std::string romulus = "fonts/romulus.ttf";
        const std::string neutra = "fonts/NeutraText-Book.otf";
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
    
    namespace anim
    {
        //transit
        const float showObjectsDuration = 0.5;
        const float toggleLightDuration = 0.5;
        const float hideObjectsDuration = 1.0;
        const float showRoomDuration = 0.5;
        const float showEntityFromGateDuration = 1.0;
        const float teamReadyEntityDelay = 0.5;
        const float teamLeftEntityDelay = 3.0;
        
        //camera
        const float camSwitchRoomDuration = 1.0;
    }
    
    namespace collision
    {
        const bool teamResetInput = false;
        
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
    
    namespace team
    {
        enum class Formation
        {
            arrow
        };
        
        const unsigned    pIndex = 1;
        const Formation   pFormation = Formation::arrow;
    }
    
    namespace camera
    {
        struct Config
        {
            float   minScale;
            float   maxScale;
            float   initScale;
            float   focusMargin;
            bool    touchMoveControl;
            bool    touchZoomControl;
        };
        
        const Config defaultConfig = {
            .minScale = 0.8,
            .maxScale = 2.0,
            .initScale = 1.0,
            .focusMargin = 50.0,
            .touchMoveControl = false,
            .touchZoomControl = false
        };
    }
}

enum class CtrlKeyType
{
    none,
    left, right, up, down,
    autoselect,
    sel1, sel2, sel3
};