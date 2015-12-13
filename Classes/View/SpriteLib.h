#pragma once

#include "SpriteEx.h"

class SpriteBlend : public SpriteEx
{
    using Base = SpriteEx;
    
public:
    //overrides
    virtual bool initWithTexture(cc::Texture2D* texture,
                                 const cc::Rect& rect,
                                 bool _rotated) override;
    virtual void draw(cc::Renderer *renderer,
                      const cc::Mat4 &transform, uint32_t flags) override;
    
    void onDraw();
    
private:
    cc::CustomCommand customCmd;
};

class SpriteGrayScale : public SpriteEx
{
    using Base = SpriteEx;
    
public:
    //overrides
    virtual bool initWithTexture(cc::Texture2D* texture,
                                 const cc::Rect& rect,
                                 bool _rotated) override;
};

