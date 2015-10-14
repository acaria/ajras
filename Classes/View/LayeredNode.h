#pragma once

#include "NodeRenderer.h"

struct LayeredNode : public cc::Node
{
    static LayeredNode* create()
    {
        LayeredNode* ret = new (std::nothrow) LayeredNode();
        if (ret && ret->init())
        {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    
    virtual bool init() override
    {
        if (!Node::init())
            return false;
    
        this->bg = cc::Layer::create();
        this->main = cc::Layer::create();
        this->main2 = cc::Layer::create();
        this->fg = cc::Layer::create();
    
        this->addChild(bg);
        this->addChild(main);
        this->addChild(main2);
        this->addChild(fg);
    
        return true;
    }
    
    NodeRenderer* getShoot(int w, int h)
    {
        return NodeRenderer::create(this, w, h);
    }
    
    cc::Layer* bg;
    cc::Layer* fg;
    cc::Layer* main;
    cc::Layer* main2;
};