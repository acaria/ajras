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
    
private:
    cc::Rect            fBounds;
    cc::RenderTexture*  buffer;
    cc::Sprite*         display;
};

