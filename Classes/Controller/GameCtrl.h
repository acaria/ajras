#pragma once

#include "Singleton.h"
#include "ScreenLog.h"
#include "TickCtrl.h"
#include "SceneManager.h"
#include "GameData.h"
#include "DebugInfoLayer.h"
#include "Event.h"
#include "ShaderManager.h"
#include "LightManager.h"

class RoomData;
class FloorData;
class PlayerData;
class GameScene;

class GameCtrl : public Singleton<GameCtrl>
{
public:
    GameCtrl();
    ~GameCtrl();

    void start();

    //navigation
    void goToMainMenu();
    void goToCamp(std::string startKey = "home");
    void goToMission();
    
    //accessors
    static GameData*        data();
    static LightManager*    light();
    TickCtrl    tick;

    //trace && debug
    template<typename ... Args>
    static void trace(const std::string& tag, const std::string& msg, Args ... args)
    {
        GameCtrl::instance()->log(LL_TRACE, tag, msg, args...);
    }
    
    template<typename ... Args>
    static void log(const std::string& msg, Args ... args)
    {
        GameCtrl::instance()->log(LL_INFO, nullptr, msg, args...);
    }
    
    template<typename ... Args>
    static void error(const std::string& msg, Args ... args)
    {
        Log(msg, args...);
        GameCtrl::instance()->log(LL_ERROR, nullptr, msg, args...);
    }

protected:
    SceneManager    scene;
    ShaderManager   shaders;
    LightManager    lightManager;

private:
    template<typename ... Args>
    void log(int level, lib::Nullable<std::string> tag, std::string msg, Args ... args)
    {
#if FOOTER_DEBUG
        size_t size = snprintf( nullptr, 0, msg.c_str(), args ... ) + 1;
        std::unique_ptr<char[]> buf( new char[ size ] );
        snprintf( buf.get(), size, msg.c_str(), args ... );
        auto text = std::string(buf.get(), buf.get() + size - 1);
        
        if (tag != nullptr && lib::hasKey(logRegs, tag.Value) &&
            logger.setMessageText(logRegs[tag.Value], text.c_str()))
            return;
        if (tag != nullptr)
            logRegs[tag.Value] = logger.log(level, text.c_str());
        else
            logger.log(level, text.c_str());
#endif
    }
    
    void onTick(double dt);
    void onAnimate(double dt, double tickPercent);
    
    GameData gameData;
    std::list<lib::Registration> eventRegs;
    
#if HEADER_DEBUG
    DebugInfoLayer* debugHeader;
#endif

#if FOOTER_DEBUG
    lib::ScreenLog                                   logger;
    std::map<std::string, lib::screenLogMessage*>    logRegs;
#endif
};