#pragma once
#include "Headers.h"

struct RoomLayer : public cocos2d::Layer
{
    static RoomLayer* create()
    {
        RoomLayer* ret = new (std::nothrow) RoomLayer();
        if (ret && ret->init())
        {
            ret->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(ret);
        }
        return ret;
    }
    
    virtual bool init() override
    {
        if (!Layer::init())
            return false;
    
        this->bg = cocos2d::Layer::create();
        this->main = cocos2d::Layer::create();
        this->fg = cocos2d::Layer::create();
    
        this->addChild(bg);
        this->addChild(main);
        this->addChild(fg);
    
        return true;
    }

    cocos2d::Layer* bg;
    cocos2d::Layer* fg;
    cocos2d::Layer* main;
};