#pragma once
#include "BaseSceneMediator.h"
#include "MissionScene.h"

class MissionMediator : public BaseSceneMediator<MissionScene>
{
public:
    virtual void onAddView(MissionScene&) override;
    virtual void onRemoveView(MissionScene&) override;
    
    static const size_t ID() {
        return std::hash<std::string>()(STRINGIFY(MissionMediator));
    };
};