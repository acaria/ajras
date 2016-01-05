#pragma once

#include "ViewNode.h"

class BufferedFrame : public ViewNode
{
    using Base = ViewNode;
public:
    BufferedFrame();
    virtual ~BufferedFrame();
    
    bool init() override;
    
    bool init(const cc::Rect& frameBounds);
    
    virtual void visit(cc::Renderer *renderer,
                       const cc::Mat4& parentTransform,
                       uint32_t parentFlags) override;
    
    PROPERTY(bool, active, Active);
    
    template<class ShaderSprite>
    void setPostProcess()
    {
        static_assert(std::is_base_of<cc::Sprite, ShaderSprite>::value,
                      "ShaderSprite must inherit from Sprite");
        this->display->release();
        this->display = cc::create<ShaderSprite>();
        this->display->retain();
        this->initDisplay();
    }
    
private:
    void initDisplay();

    cc::Rect            fBounds;
    cc::RenderTexture*  buffer;
    cc::Sprite*         display;
};

