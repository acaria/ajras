#include "LayeredContainer.h"
#include "SpriteEx.h"
#include "ShaderLayer.h"
#include "SpriteLib.h"

bool LayeredContainer::init(cc::Size size)
{
    if (!Base::init())
        return false;
    
    this->bg = cc::Layer::create();
    this->bg->setTag((int)def::LayerType::BG);
    
    this->main = cc::Layer::create();
    this->main->setTag((int)def::LayerType::MAIN1);
    
    this->main2 = cc::Layer::create();
    this->main2->setTag((int)def::LayerType::MAIN2);
    
    this->fg = cc::Layer::create();
    this->fg->setTag((int)def::LayerType::FG);
    
    Base::addChild(bg,      1,  (int)def::LayerType::BG);
    Base::addChild(main,    2,  (int)def::LayerType::MAIN1);
    Base::addChild(main2,   3,  (int)def::LayerType::MAIN2);
    Base::addChild(fg,      4,  (int)def::LayerType::FG);
    
    this->size = size;
    
    return true;
}

void LayeredContainer::rasterizeBg()
{
    this->bg->retain();
    Base::removeChild(bg);
    
    auto bgPlan = NodeRenderer::create(this->bg, size.width, size.height);
    Base::addChild(bgPlan, 1);
    this->bg->release();
}

NodeRenderer* LayeredContainer::createShot(int w, int h)
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

void LayeredContainer::add(cc::Node* node, def::LayerType type, const cc::Point& zMargin)
{
    this->get(type)->addChild(node, getZOrder(zMargin + node->getPosition()));
    
    auto spriteEx = dynamic_cast<SpriteEx*>(node);
    if (spriteEx != nullptr)
        spriteEx->setContainer(this, zMargin);
}

int LayeredContainer::getZOrder(const cc::Vec2& pos)
{
    return (this->size.width - pos.x +
        ((this->size.height - pos.y) * this->size.height));
}