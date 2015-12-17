#include "BufferedFrame.h"

BufferedFrame::BufferedFrame()
{
    this->active = true;
}

BufferedFrame::~BufferedFrame()
{
    if (this->display != nullptr)
        this->display->release();
    if (this->buffer != nullptr)
        this->buffer->release();
}

void BufferedFrame::visit(cc::Renderer *renderer,
                          const cc::Mat4& parentTransform,
                          uint32_t parentFlags)
{
    if (this->active)
    {
        this->buffer->beginWithClear(0, 0, 0, 0);
        cc::Mat4 localTransform;
        parentTransform.translate(-fBounds.origin.x, -fBounds.origin.y, 0, &localTransform);
        Base::visit(renderer, localTransform, parentFlags);
        this->buffer->end();
        this->display->visit(renderer, parentTransform, parentFlags);
    }
    else //disabled: visit directly without buffer
    {
        Base::visit(renderer, parentTransform, parentFlags);
    }
}

bool BufferedFrame::init()
{
    return false;
}

bool BufferedFrame::init(const cc::Rect& fBounds)
{
    if (!Base::init())
        return false;
    this->setAnchorPoint({0,0});
    this->fBounds = fBounds;
    
    //init buffer
    this->buffer = cc::RenderTexture::create(fBounds.size.width, fBounds.size.height);
    this->buffer->setAnchorPoint({0,0});
    this->buffer->retain();
    
    //init display
    this->display = cc::create<cc::Sprite>();
    this->display->retain();
    this->initDisplay();
    
    return true;
}

void BufferedFrame::initDisplay()
{
    this->display->setTexture(this->buffer->getSprite()->getTexture());
    this->display->setTextureRect({0,0,fBounds.size.width,fBounds.size.height});
    this->display->setPosition({0,0});
    this->display->setFlippedY(true);
    this->display->setAnchorPoint({0,0});
}