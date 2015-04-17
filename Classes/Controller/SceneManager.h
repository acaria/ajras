#pragma once
#include "ScreenLog.h"
#include "BaseMediator.h"
#include "EventScene.h"

USING_NS_CC;

class SceneManager
{
public:
    SceneManager(ScreenLog &log);
    ~SceneManager();
    void go2MainMenu();
    void go2Game();

protected:

    void init();
    
    template<class Mediator> void launch() {
        static_assert(std::is_base_of<BaseMediator, Mediator>::value,
                      "Mediator doesn't inherit from BaseMediator");
        
        if (mediators.find(Mediator::ID()) == mediators.end())
            mediators[Mediator::ID()] = new Mediator();
        auto currentMediator = static_cast<Mediator*>(mediators[Mediator::ID()]);
        
        auto scene = currentMediator->createView();
        assert(scene);
        
        if (currentScene)
        {
            currentScene->onLeaveBeforeTransition();
            currentScene->autorelease();
        }
        
        currentScene = scene;
        currentScene->retain();
        
        eventRegs.push_back(scene->onEnterBeforeTransition.registerObserver([currentMediator, scene](){
            currentMediator->triggerAddView(*scene);
        }));

        eventRegs.push_back(scene->onLeaveBeforeTransition.registerObserver([currentMediator, scene](){
            currentMediator->triggerRemoveView(*scene);
        }));
        
        this->screenLog->attachToScene(scene);
        
        if (firstTime)
        {
            firstTime = false;
            Director::getInstance()->runWithScene(scene);
        }
        else
        {
            auto transition = TransitionFade::create(1.0f, scene, Color3B::BLACK);
            Director::getInstance()->replaceScene(transition);
        }
    }

private:
    bool                            firstTime = true;
    std::map<size_t, BaseMediator*> mediators;
    std::vector<event::Registration> eventRegs;
    EventScene*                     currentScene;

    ScreenLog*                      screenLog;
};