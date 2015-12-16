#pragma once

#include "Defines.h"

class LayeredContainer;

class SpriteEx : public cc::Sprite
{
    using Base = cc::Sprite;

public:
    virtual ~SpriteEx(){}
    void setContainer(LayeredContainer* container, cc::Point zMargin);
    
    //overrides
    void setPosition(const cc::Vec2 &pos) override;
    void setPosition(float x, float y) override;
    const cc::Vec2& getPosition() const override;
    cc::Vec3 getPosition3D() const override;
    bool initWithTexture(cc::Texture2D *texture, const cc::Rect &rect, bool _rotated) override;

    //getters
    LayeredContainer* getContainer() { return this->container; }
    
private:
    LayeredContainer* container = nullptr;
    cc::Point zMargin = {0,0};
    cc::Point actualPosition = {0,0};
};
