#pragma once
#include "ScreenLog.h"
#include "TickCtrl.h"
#include "BaseMediator.h"
#include "EventScene.h"

USING_NS_CC;

class SceneManager
{
public:
    SceneManager(const TickCtrl &tickCtrl);
    ~SceneManager();
    void go2MainMenu();
    void go2Mission();
    void go2Camp();
    
    void onTick(double dt);
    void onAnimate(double dt, double tickPercent);

protected:

    void init();
    
    template<class Mediator> void launch() {
        static_assert(std::is_base_of<BaseMediator, Mediator>::value,
                      "Mediator doesn't inherit from BaseMediator");
        
        if (mediators.find(Mediator::ID()) == mediators.end())
            mediators[Mediator::ID()] = new Mediator();
        auto cMediator = static_cast<Mediator*>(mediators[Mediator::ID()]);
        auto scene = cMediator->createView();
        assert(scene);
        
        if (currentScene)
        {
            currentScene->onLeaveBeforeTransition();
            currentScene->autorelease();
        }
        
        currentScene = scene;
        currentScene->retain();
        
        eventRegs.push_back(scene->onEnterBeforeTransition.registerObserver([cMediator, scene, this](){
            cMediator->triggerAddView(*scene);
            this->tickCtrl.schedule(scene);
            this->currentMediator = cMediator;
        }));

        eventRegs.push_back(scene->onLeaveBeforeTransition.registerObserver([cMediator, scene, this](){
            cMediator->triggerRemoveView(*scene);
            this->currentMediator = nullptr;
        }));
        
        //this->screenLog->attachToScene(scene);
        
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
    std::vector<lib::Registration>  eventRegs;
    
    EventScene*                     currentScene = nullptr;
    BaseMediator*                   currentMediator = nullptr;

    TickCtrl                        tickCtrl;
};