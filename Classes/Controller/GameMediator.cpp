#include "GameMediator.h"
#include "GameScene.h"
#include "GameCtrl.h"

void GameMediator::onAddView(GameScene& scene)
{
    GameCtrl::instance()->loadSession(&scene);
    
    this->eventRegs.push_back(scene.onEnterAfterTransition.registerObserver([](){
        CCLOG("game transition finished");
    }));
}

void GameMediator::onRemoveView(GameScene &scene)
{
}