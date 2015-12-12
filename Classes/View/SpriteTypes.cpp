#include "SpriteTypes.h"

bool SpriteBlend::initWithTexture(cc::Texture2D *texture, const cc::Rect &rect, bool _rotated)
{
    auto ret = cc::Sprite::initWithTexture(texture, rect, _rotated);
    
    auto ps = cc::GLProgramState::getOrCreateWithGLProgram(
            cc::GLProgramCache::getInstance()->getGLProgram("blend"));
    this->setGLProgramState(ps);
    
    this->customCmd.init(_globalZOrder);
    this->customCmd.func = std::bind(&SpriteBlend::onDraw, this);
    
    return ret;
}

void SpriteBlend::draw(cc::Renderer *renderer, const cc::Mat4 &transform, uint32_t flags)
{
    if (this->getGLProgramState())
    {
        renderer->addCommand(&customCmd);
    }
    
    Sprite::draw(renderer, transform, flags);
}

void SpriteBlend::onDraw()
{
    auto ps = this->getGLProgramState();
    
    auto texSize = this->getTexture()->getContentSizeInPixels();
    auto texRect = this->getTextureRect();
    
    ps->setUniformVec2("screenSize", cc::Director::getInstance()->getWinSize());
    ps->setUniformVec2("blendCenter", {0,0});
    ps->setUniformVec2("spritePosition", this->getPosition());
    ps->setUniformVec2("spriteFrameOrigin", {texRect.origin.x / texSize.width,
        texRect.origin.y / texSize.height});
    ps->setUniformVec2("spriteFrameSize", this->getTexture()->getContentSizeInPixels());
}
