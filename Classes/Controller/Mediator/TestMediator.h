#pragma once

#include "BaseSceneMediator.h"

template<class Scene>
class TestMediator : public BaseSceneMediator<Scene>
{
public:
    virtual void onAddView(Scene&) {}
    
    virtual void onRemoveView(Scene&) {}
    
    static const size_t ID() {
        return std::hash<std::string>()(STRINGIFY(Scene));
    };
};

