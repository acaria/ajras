#include "MissionMediator.h"
#include "MissionScene.h"
#include "GameCtrl.h"

void MissionMediator::onAddView(MissionScene &scene)
{
    floorSystemCtrl.load(&scene, GameCtrl::instance()->getData().curFloor());
    floorSystemCtrl.start();
    
    this->eventRegs.push_back(scene.onEnterAfterTransition.registerObserver([](){
        Log("MissionScene after");
    }));
    
    this->eventRegs.push_back(scene.onMenuCmd.registerObserver(
        [&scene](MissionScene::CmdTag tag) {
            switch(tag)
            {
                case MissionScene::CmdTag::QUIT:
                    GameCtrl::instance()->goToMainMenu();
                    break;
                default:
                    Log("unsupported ItemTag in MissionMediator");
                    break;
            }
    }));
}

void MissionMediator::onRemoveView(MissionScene &scene)
{
}

void MissionMediator::onTick(double dt)
{
    floorSystemCtrl.tick(dt);
}

void MissionMediator::onAnimate(double dt, double tickPercent)
{
    floorSystemCtrl.animate(dt, tickPercent);
}