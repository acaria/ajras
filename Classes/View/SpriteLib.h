#pragma once

#include "ShaderSprite.h"
#include "SpriteEx.h"

class SpriteBlend : public ShaderSprite<SpriteEx>
{
public:
    SpriteBlend() : ShaderSprite("blend", true) {};
    
protected:
    virtual void setCustomUniforms(cc::GLProgramState* glState,
                                   const cc::Mat4 &transform,
                                   uint32_t flags) override;
};

class SpriteGrayScale : public ShaderSprite<SpriteEx>
{
public:
    SpriteGrayScale() : ShaderSprite("greyscale", false) {}
};

class SpriteTest : public ShaderSprite<SpriteEx>
{
public:
    SpriteTest() : ShaderSprite("test", false) {}
};

class SpriteLight : public ShaderSprite<SpriteEx>
{
public:
    SpriteLight() : ShaderSprite("light", true) {
        auto pOrigin = cc::Director::getInstance()->getVisibleOrigin();
        auto pSize = cc::Director::getInstance()->getVisibleSize();
        auto margin = def::canvasMissionMargin;
        
        this->frameBounds = { pOrigin.x + margin.x,
                              pOrigin.y + margin.w,
                              pSize.width - margin.x - margin.y,
                              pSize.height - margin.w - margin.z };
    }
    
protected:
    virtual void setCustomUniforms(cc::GLProgramState* glState,
                                   const cc::Mat4 &transform,
                                   uint32_t flags) override;
    
private:
    cc::Rect frameBounds;
    std::vector<float> spot;
};