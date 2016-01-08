#include "CocosHelper.h"

void CocosHelper::addTexture(const std::string& plistFileName, bool antialias)
{
    cc::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistFileName);
    
    auto pngFileName = plistFileName;
    size_t startPos = pngFileName.find_last_of(".");
    pngFileName = pngFileName.erase(startPos);
    pngFileName = pngFileName.append(".png");
    
    auto spriteFrame = cc::Director::getInstance()->getTextureCache();
    if (antialias)
        spriteFrame->getTextureForKey(pngFileName)->setAntiAliasTexParameters();
    else
        spriteFrame->getTextureForKey(pngFileName)->setAliasTexParameters();
}

cc::ActionInterval* CocosHelper::blinkActionCreate(const cc::Color3B& color, unsigned count, float duration)
{
    auto df = duration / count / 2;
    return cc::Repeat::create(
        cc::Sequence::create(
            cc::TintTo::create(df, color.r, color.g, color.b),
            cc::TintTo::create(df, 255, 255, 255),
            NULL),
        count);
}

void CocosHelper::setRecursiveCascadeOpacity(cc::Node *node, bool value)
{
    node->setCascadeOpacityEnabled(true);
    for(auto child : node->getChildren())
    {
        setRecursiveCascadeOpacity(child, value);
    }
}