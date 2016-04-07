#pragma once
#include "LightConfig.h"
#include "CsActionInterval.h"

struct LightSpotComponent
{
    cc::Sprite*                         halo = nullptr;
    std::pair<cc::Size, cc::Size>       defaultSize;
    std::pair<cc::Color3B, cc::Color3B> defaultColor;
    std::pair<int, int>                 defaultOpacity;
    
    cc::Action* createFlickerAction()
    {
        assert(halo != nullptr);
        
        cc::Vec2 scaleTarget1 = {
            defaultSize.first.width / halo->getContentSize().width,
            defaultSize.first.height / halo->getContentSize().height,
        };
        cc::Vec2 scaleTarget2 = {
            defaultSize.second.width / halo->getContentSize().width,
            defaultSize.second.height / halo->getContentSize().height,
        };
        auto step1 = cc::Spawn::create(
            cc::ActionFloat::create(
                def::anim::toggleLightDuration,
                halo->getScaleX(),
                scaleTarget1.x,
                [this](float v){ halo->setScaleX(v); }),
            cc::ActionFloat::create(
                def::anim::toggleLightDuration,
                halo->getScaleY(),
                scaleTarget1.y,
                [this](float v){ halo->setScaleY(v); }),
            NULL);
        
        auto step2 = cc::CallFunc::create(
            [this, scaleTarget1, scaleTarget2](){
                halo->runAction(cc::RepeatForever::create(Flicker::create(
                    1, 0.1,
                    {
                        (float)defaultOpacity.first,
                        (float)defaultOpacity.second
                    },
                    {
                        scaleTarget1.x,
                        scaleTarget2.x
                    },
                    {
                        scaleTarget1.y,
                        scaleTarget2.y
                    },
                    defaultColor.first, defaultColor.second)));
        });
        
        return cc::Sequence::create(step1, step2, NULL);
    }
};

