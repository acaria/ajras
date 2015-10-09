#include "CampScene.h"

CampScene::~CampScene()
{
    
}

bool CampScene::init()
{
    if (!Base::init())
        return false;
    
    return true;
}