#pragma once

#include "BaseSceneMediator.h"
#include "CampScene.h"
#include "CampSystemCtrl.h"

class CampMediator : public BaseSceneMediator<CampScene>
{
public:
    virtual void onAddView(CampScene&) override;
    virtual void onRemoveView(CampScene&) override;
    
    static const size_t ID() {
        return std::hash<std::string>()(STRINGIFY(CampMediator));
    };
    
    virtual void onTick(double dt) override;
    virtual void onAnimate(double dt, double tickPercent) override;
    
private:
    void registerDispatcher(CampScene& scene);

    CampSystemCtrl                  systemCtrl;
    std::list<lib::Registration>    systemRegs;
};
