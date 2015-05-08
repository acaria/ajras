#include "NodeRenderer.h"

NodeRenderer* NodeRenderer::create(cc::Node* node, int w, int h)
{
    NodeRenderer* renderer = new (std::nothrow) NodeRenderer();
    if(renderer && renderer->initWithNode(node, w, h))
    {
        renderer->autorelease();
        return renderer;
    }
    CC_SAFE_DELETE(renderer);
    return nullptr;
}

void NodeRenderer::renderChildren()
{
    this->begin();
    for(auto child : this->getChildren())
    {
        child->visit();
    }
    this->end();
}

bool NodeRenderer::initWithNode(cc::Node *node, int w, int h)
{
    if (!this->initWithWidthAndHeight(w, h, cc::Texture2D::PixelFormat::RGBA8888))
        return false;
    
    this->addChild(node);
    this->getSprite()->setAnchorPoint({0,0});
    this->getSprite()->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
    
    this->renderChildren();
    
    return true;
}