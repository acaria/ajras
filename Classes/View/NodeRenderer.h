#pragma once

class NodeRenderer : public cc::RenderTexture
{
public:
    virtual ~NodeRenderer() {}

    static NodeRenderer* create(cc::Node* node, int w, int h);
    
protected:
    bool initWithNode(cc::Node*, int w, int h);
    
private:
    void renderChildren();
};

