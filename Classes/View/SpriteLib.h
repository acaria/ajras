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
    SpriteLight() : ShaderSprite("light", true) {}
    
protected:
    virtual void setCustomUniforms(cc::GLProgramState* glState,
                                   const cc::Mat4 &transform,
                                   uint32_t flags) override;

private:
    void genSpotsList();

    int                 nbSpots;
    std::vector<float>  spotList;
};