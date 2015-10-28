#pragma once

#include "NodeRenderer.h"
#include "LayeredSprite.h"
#include "Defines.h"

class LayeredContainer : public cc::Node
{
public:
    bool init() override
    {
        Log("invalid layered container initialisation, size is missing");
        return false;
    }
    
    bool init(cc::Size size);
    
    int getZOrder(const cc::Vec2& pos);
    NodeRenderer* getShoot(int w, int h);
    
    LayeredSprite* createChild(const std::string& frameName, def::LayerType type, cc::Point zMargin = {0,0});
    
    cc::Layer* get(def::LayerType type);
    
private:
    cc::Layer* bg;
    cc::Layer* fg;
    cc::Layer* main;
    cc::Layer* main2;

    cc::Size    size;
};