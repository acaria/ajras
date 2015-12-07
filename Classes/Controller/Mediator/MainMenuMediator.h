#pragma once

#include "BaseSceneMediator.h"
#include "MainMenuScene.h"

class MainMenuMediator : public BaseSceneMediator<MainMenuScene>
{
public:
    virtual void onAddView(MainMenuScene&);
    virtual void onRemoveView(MainMenuScene&);
    
    static const size_t ID() {
        return std::hash<std::string>()(STRINGIFY(MainMenuMediator));
    };
};
