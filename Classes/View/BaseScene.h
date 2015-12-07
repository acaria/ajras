#pragma once
#include "EventScene.h"

template <class T>
class BaseScene : public EventScene
{
public:
    virtual ~BaseScene() {}
    
    static T* create()
    {
        T *pRet = new T();
        if (pRet && pRet->init())
        {
            pRet->autorelease();
            return pRet;
        }
        else
        {
            Log("cannot create scene");
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }
    
    virtual bool init() override { return cocos2d::Scene::init(); };
    
protected:
    typedef BaseScene<T> Base;
};
