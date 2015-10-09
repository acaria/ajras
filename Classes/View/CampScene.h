#pragma once

#include "BaseScene.h"

class CampScene : public BaseScene<CampScene>
{
public:
    virtual bool init() override;
    virtual ~CampScene();
    
    lib::Subject<void()> onBack;
    
    //background
    void setBgColor(cc::Color3B bgColor);
    
private:
    
};
