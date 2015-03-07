#pragma once
#include "Headers.h"

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
        onAddView((Scene&)scene);
    }
    
    void triggerRemoveView(cocos2d::Scene& scene) override
    {
        BaseMediator::triggerRemoveView(scene);
        onRemoveView((Scene&)scene);
    }
};
