#pragma once
#include "BaseMediator.h"
#include "GameCtrl.h"

#define STRINGIFY(x) #x

template<class Scene>
class BaseSceneMediator: public BaseMediator
{
public:
    virtual ~BaseSceneMediator() {};
    
    virtual void onAddView(Scene&) = 0;
    virtual void onRemoveView(Scene&) = 0;
    
    Scene* createView() {
        return Scene::create();
    }
    
    void triggerAddView(cocos2d::Scene& scene) override
    {
        auto kListener = cc::EventListenerKeyboard::create();
        kListener->onKeyReleased = [this](KeyCode code, cocos2d::Event *event) {
            if (code == KeyCode::KEY_GRAVE)
                GameCtrl::instance()->getData().toggleDebug();
        };
        scene.getEventDispatcher()->addEventListenerWithSceneGraphPriority(kListener, &scene);
        
        onAddView((Scene&)scene);
    }
    
    void triggerRemoveView(cocos2d::Scene& scene) override
    {
        BaseMediator::triggerRemoveView(scene);
        onRemoveView((Scene&)scene);
    }
};
