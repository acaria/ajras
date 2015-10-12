#include "CampMediator.h"
#include "GameCtrl.h"

void CampMediator::onAddView(CampScene &scene)
{
    auto campData = GameCtrl::instance()->getData().curCamp();
    auto playerData = GameCtrl::instance()->getData().curPlayer();
    
    scene.interface->registerPlayer(playerData->ctrlIndex, [playerData](KeyCode code) {
        return playerData->KeyCode2KeyType(code);
    });
}

void CampMediator::onRemoveView(CampScene &scene)
{
    scene.getEventDispatcher()->removeAllEventListeners();
}

void CampMediator::onTick(double dt)
{
}

void CampMediator::onAnimate(double dt, double tickPercent)
{
}