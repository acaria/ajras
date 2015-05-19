#include "GameMediator.h"
#include "GameScene.h"
#include "GameCtrl.h"

void GameMediator::onAddView(GameScene& scene)
{
    GameCtrl::instance()->startSession(&scene);
    this->eventRegs.push_back(scene.onEnterAfterTransition.registerObserver([](){
        Log("GameScene after");
    }));
}

void GameMediator::onRemoveView(GameScene &scene)
{
}