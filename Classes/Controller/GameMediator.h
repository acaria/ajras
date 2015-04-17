#pragma once
#include "BaseSceneMediator.h"
#include "GameScene.h"

class GameMediator : public BaseSceneMediator<GameScene>
{
public:
    virtual void onAddView(GameScene&) override;
    virtual void onRemoveView(GameScene&) override;
    
    static const size_t ID() {
        return std::hash<std::string>()(STRINGIFY(GameMediator));
    };    
};