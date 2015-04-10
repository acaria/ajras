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
    
        this->setCascadeOpacityEnabled(true);
        this->bg = cc::Layer::create();
        this->bg->setCascadeOpacityEnabled(true);
        this->main = cc::Layer::create();
        this->main->setCascadeOpacityEnabled(true);
        this->fg = cc::Layer::create();
        this->fg->setCascadeOpacityEnabled(true);
    
        this->addChild(bg);
        this->addChild(main);
        this->addChild(fg);
    
        this->setOpacity(0);
    
        return true;
    }

    cocos2d::Layer* bg;
    cocos2d::Layer* fg;
    cocos2d::Layer* main;
};