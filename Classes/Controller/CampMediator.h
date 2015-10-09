#pragma once

#include "BaseSceneMediator.h"
#include "CampScene.h"

class CampMediator : public BaseSceneMediator<CampScene>
{
public:
    virtual void onAddView(CampScene&);
    virtual void onRemoveView(CampScene&);
    
    static const size_t ID() {
        return std::hash<std::string>()(STRINGIFY(CampMediator));
    };
};
