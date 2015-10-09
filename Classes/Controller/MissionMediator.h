#pragma once
#include "BaseSceneMediator.h"
#include "MissionScene.h"
#include "FloorSystemCtrl.h"

class MissionMediator : public BaseSceneMediator<MissionScene>
{
public:
    virtual void onAddView(MissionScene&) override;
    virtual void onRemoveView(MissionScene&) override;
    
    static const size_t ID() {
        return std::hash<std::string>()(STRINGIFY(MissionMediator));
    };
    
    virtual void onTick(double dt) override;
    virtual void onAnimate(double dt, double tickPercent) override;
    
private:
    FloorSystemCtrl  floorSystemCtrl;
};