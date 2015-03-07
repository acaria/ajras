#pragma once
#include "Headers.h"

class GameMediator : public BaseSceneMediator<GameScene>
{
public:
    virtual void onAddView(GameScene&) override;
    virtual void onRemoveView(GameScene&) override;
    
    static const size_t ID() {
        return std::hash<std::string>()(STRINGIFY(GameMediator));
    };    
};