#pragma once

struct RoomLayer : public cc::Layer
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

    cc::Layer* bg;
    cc::Layer* fg;
    cc::Layer* main;
};