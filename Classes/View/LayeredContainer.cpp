#include "LayeredContainer.h"
#include "LayeredSprite.h"

bool LayeredContainer::init(cc::Size size)
{
    if (!Node::init())
        return false;
    
    this->bg = cc::Layer::create();
    this->bg->setTag((int)def::LayerType::BG);
    
    this->main = cc::Layer::create();
    this->main->setTag((int)def::LayerType::MAIN1);
    
    this->main2 = cc::Layer::create();
    this->main2->setTag((int)def::LayerType::MAIN2);
    
    this->fg = cc::Layer::create();
    this->fg->setTag((int)def::LayerType::FG);
    
    Node::addChild(bg);
    Node::addChild(main);
    Node::addChild(main2);
    Node::addChild(fg);
    
    this->size = size;
    
    return true;
}

NodeRenderer* LayeredContainer::getShoot(int w, int h)
{
    return NodeRenderer::create(this, w, h);
}

cc::Layer* LayeredContainer::get(def::LayerType type)
{
    switch(type)
    {
        case def::LayerType::BG: return bg;
        case def::LayerType::FG: return fg;
        case def::LayerType::MAIN1: return main;
        case def::LayerType::MAIN2: return main2;
        default:
        {
            Log("invalid layer type");
            return nullptr;
        }
    }
}

int LayeredContainer::getZOrder(const cc::Vec2& pos)
{
    return (this->size.width - pos.x +
        ((this->size.height - pos.y) * this->size.height));
}

LayeredSprite* LayeredContainer::createChild(const std::string& frameName,
                                             def::LayerType type,
                                             cc::Point zMargin)
{
    auto sprite = LayeredSprite::create(frameName, this, zMargin);
    sprite->setAnchorPoint({0,0});
    this->get(type)->addChild(sprite);
    return sprite;
}