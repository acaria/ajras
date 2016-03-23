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

void SpriteLight::genSpotsList()
{
    auto frameBounds = GameCtrl::instance()->getEffects().getFrame();
    auto spots = GameCtrl::instance()->getEffects().getLightConfig().spots;
    this->nbSpots = spots.size();
    this->spotList = std::vector<float>();
    if (this->nbSpots > 0)
    {
        for(auto spot : spots)
        {
            cc::Vec2 lightPos = {spot.pos.x - frameBounds.origin.x,
                                 spot.pos.y - frameBounds.origin.y};
            lightPos.y = frameBounds.size.height - lightPos.y;
            
            cc::Point posRelToSprite = PointApplyAffineTransform(
                cc::Point(lightPos.x, lightPos.y), this->getWorldToNodeAffineTransform());
            
            this->spotList.push_back(spot.brightness);
            this->spotList.push_back(spot.color.r / 255.0f);
            this->spotList.push_back(spot.color.g / 255.0f);
            this->spotList.push_back(spot.color.b / 255.0f);
            this->spotList.push_back(posRelToSprite.x);
            this->spotList.push_back(posRelToSprite.y);
            this->spotList.push_back(spot.pos.z);
            this->spotList.push_back(spot.cutOffRadius);
            this->spotList.push_back(spot.halfRadius);
        }
    }
}

void SpriteLight::setCustomUniforms(cc::GLProgramState *gls,
                                    const cc::Mat4 &transform,
                                    uint32_t flags)
{
    cc::SpriteFrame *frame = this->getSpriteFrame();
    cc::Size untrimmedSize = frame->getOriginalSize();
    cc::Size trimmedSize = frame->getRect().size;
    cc::Vec2 framePos = frame->getRect().origin;
    cc::Size texSize = frame->getTexture()->getContentSize();
    
    this->genSpotsList();
    
    //set lights
    gls->setUniformInt("u_nbLights", this->nbSpots);
    gls->setUniformFloatv("u_spotLight", this->spotList.size(), &this->spotList[0]);
    //----
    
    // set texture
    gls->setUniformVec2("u_contentSize", this->getContentSize());
    gls->setUniformVec2("u_spritePosInSheet", {framePos.x / texSize.width,
                                               framePos.y / texSize.height});
    gls->setUniformVec2("u_spriteSizeRelToSheet", {untrimmedSize.width / texSize.width,
                                                   untrimmedSize.height / texSize.height});
    gls->setUniformInt("u_spriteRotated", frame->isRotated());
    cc::Vec2 bottomLeft = frame->getOffset() + (untrimmedSize - trimmedSize) / 2;
    cc::Vec2 cornerOffset = frame->isRotated() ? cc::Vec2(bottomLeft.y, bottomLeft.x)
        : cc::Vec2(bottomLeft.x, untrimmedSize.height - trimmedSize.height - bottomLeft.y);
    gls->setUniformVec2("u_spriteOffset", cornerOffset);
}