#pragma once

#include "NodeRenderer.h"
#include "Defines.h"

class LayeredContainer : public cc::Node
{
    using Base = cc::Node;
public:
    bool init() override
    {
        Log("invalid layered container initialisation, size is missing");
        return false;
    }
    
    bool init(cc::Size size);
    
    int getZOrder(const cc::Vec2& pos);
    NodeRenderer* createShot(int w, int h);
    
    //getters
    cc::Layer* get(def::LayerType type);
    
    void add(cc::Node* node, def::LayerType type, const cc::Point& zMargin = {0,0});
    
private:
    cc::Layer* bg;
    cc::Layer* fg;
    cc::Layer* main;
    cc::Layer* main2;

    cc::Size    size;
};