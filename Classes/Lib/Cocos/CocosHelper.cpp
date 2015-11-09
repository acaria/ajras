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