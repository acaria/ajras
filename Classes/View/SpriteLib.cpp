#include "SpriteLib.h"
#include "GameCtrl.h"

void SpriteBlend::setCustomUniforms(cc::GLProgramState* glState,
                                    const cc::Mat4 &transform,
                                    uint32_t flags)
{
    auto texSize = this->getTexture()->getContentSizeInPixels();
    auto texRect = this->getTextureRect();
    
    //ps->setUniformVec2("screenSize", cc::Director::getInstance()->getWinSize());
    //ps->setUniformVec2("blendCenter", {0,0});
    //ps->setUniformVec2("spritePosition", this->getPosition());
    //ps->setUniformVec2("spriteFrameOrigin", {texRect.origin.x / texSize.width,
    //                                         texRect.origin.y / texSize.height});
    //ps->setUniformVec2("spriteFrameSize", this->getTexture()->getContentSizeInPixels());
}

void SpriteLight::setCustomUniforms(cc::GLProgramState *gls,
                                    const cc::Mat4 &transform,
                                    uint32_t flags)
{
    auto lightPos = GameCtrl::instance()->getEffects().getLightPos();

    //dynamics
    gls->setUniformVec2("u_contentSize", this->getContentSize());
    
    cc::Point posRelToSprite = PointApplyAffineTransform(
            cc::Point(lightPos.x, lightPos.y), this->getWorldToNodeAffineTransform());
    gls->setUniformVec3("u_lightPos", cc::Vec3(posRelToSprite.x, posRelToSprite.y, lightPos.z));
    
    //gls->setUniformTexture("u_normals", normalmap);
    
    cc::SpriteFrame *frame = this->getSpriteFrame();
    cc::Size untrimmedSize = frame->getOriginalSize();
    cc::Size trimmedSize = frame->getRect().size;
    cc::Vec2 framePos = frame->getRect().origin;
    cc::Size texSize = frame->getTexture()->getContentSize();
    
    // set sprite position in sheet
    gls->setUniformVec2("u_spritePosInSheet", {framePos.x / texSize.width,
                                               framePos.y / texSize.height});
    gls->setUniformVec2("u_spriteSizeRelToSheet", {untrimmedSize.width / texSize.width,
                                                   untrimmedSize.height / texSize.height});
    gls->setUniformInt("u_spriteRotated", frame->isRotated());
    
    // set offset of trimmed sprite
    cc::Vec2 bottomLeft = frame->getOffset() + (untrimmedSize - trimmedSize) / 2;
    cc::Vec2 cornerOffset = frame->isRotated() ? cc::Vec2(bottomLeft.y, bottomLeft.x)
        : cc::Vec2(bottomLeft.x, untrimmedSize.height - trimmedSize.height - bottomLeft.y);
    gls->setUniformVec2("u_spriteOffset", cornerOffset);
}

void BufferLight::setCustomUniforms(cc::GLProgramState *gls,
                                    const cc::Mat4 &transform,
                                    uint32_t flags)
{
    auto lightPos = GameCtrl::instance()->getEffects().getLightPos();
    auto frameBounds = GameCtrl::instance()->getEffects().getFrame();
    
    lightPos = { lightPos.x - frameBounds.origin.x, lightPos.y - frameBounds.origin.y, lightPos.z};
    lightPos.y = frameBounds.size.height - lightPos.y;
    
    //dynamics
    gls->setUniformVec2("u_contentSize", this->getContentSize());
    
    cc::Point posRelToSprite = PointApplyAffineTransform(cc::Point(lightPos.x, lightPos.y),
                                                         this->getWorldToNodeAffineTransform());
    gls->setUniformVec3("u_lightPos", cc::Vec3(posRelToSprite.x, posRelToSprite.y, lightPos.z));
    
    //gls->setUniformTexture("u_normals", normalmap);
    
    cc::SpriteFrame *frame = this->getSpriteFrame();
    cc::Size untrimmedSize = frame->getOriginalSize();
    cc::Size trimmedSize = frame->getRect().size;
    cc::Vec2 framePos = frame->getRect().origin;
    cc::Size texSize = frame->getTexture()->getContentSize();
    
    // set sprite position in sheet
    gls->setUniformVec2("u_spritePosInSheet", {framePos.x / texSize.width,
        framePos.y / texSize.height});
    gls->setUniformVec2("u_spriteSizeRelToSheet", {untrimmedSize.width / texSize.width,
        untrimmedSize.height / texSize.height});
    gls->setUniformInt("u_spriteRotated", frame->isRotated());
    
    // set offset of trimmed sprite
    cc::Vec2 bottomLeft = frame->getOffset() + (untrimmedSize - trimmedSize) / 2;
    cc::Vec2 cornerOffset = frame->isRotated() ? cc::Vec2(bottomLeft.y, bottomLeft.x)
    : cc::Vec2(bottomLeft.x, untrimmedSize.height - trimmedSize.height - bottomLeft.y);
    gls->setUniformVec2("u_spriteOffset", cornerOffset);
}

