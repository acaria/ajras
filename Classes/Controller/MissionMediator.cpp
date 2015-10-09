#include "MissionMediator.h"
#include "MissionScene.h"
#include "GameCtrl.h"

void MissionMediator::onAddView(MissionScene& scene)
{
    GameCtrl::instance()->startSession(&scene);
    this->eventRegs.push_back(scene.onEnterAfterTransition.registerObserver([](){
        Log("GameScene after");
    }));
}

void MissionMediator::onRemoveView(MissionScene &scene)
{
}