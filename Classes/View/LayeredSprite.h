#pragma once

#include "Defines.h"

class LayeredContainer;

class LayeredSprite : public cc::Sprite
{
public:
    static LayeredSprite* create(const std::string& frameName,
                                 LayeredContainer* container,
                                 cc::Point zMargin);
    
    void switchContainer(LayeredContainer* container);
    void switchLayer(def::LayerType type);
    
    
    virtual void setPosition(const cc::Vec2& pos) override;
    virtual void setPosition(float x, float y) override;
    LayeredContainer* getContainer();
    
private:
    
    static LayeredSprite* createWithSpriteFrame(cc::SpriteFrame *spriteFrame);
    static LayeredSprite* createWithSpriteFrameName(const std::string& spriteFrameName);
    
    LayeredContainer* container;
    cc::Point zMargin;
};
