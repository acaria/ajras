#include "CampSystemCtrl.h"
#include "Components.h"
#include "PlayerData.h"

CampSystemCtrl::CampSystemCtrl() : ctrlCampSystem(ecsGroup),
                                   random(Randgine::instance()->get(Randgine::CAMP))
{
    
}

CampSystemCtrl::~CampSystemCtrl()
{
    
}

void CampSystemCtrl::tick(double dt)
{
    ctrlCampSystem.tick(dt);
    
    
}

void CampSystemCtrl::animate(double dt, double tickPercent)
{
    ctrlCampSystem.animate(dt, tickPercent);
}