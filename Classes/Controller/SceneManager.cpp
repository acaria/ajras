#include "SceneManager.h"
#include "MainMenuMediator.h"
#include "MissionMediator.h"

SceneManager::SceneManager(const TickCtrl& tickCtrl) : tickCtrl(tickCtrl)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::go2MainMenu()
{
    this->launch<MainMenuMediator>();
}

void SceneManager::go2Mission()
{
    this->launch<MissionMediator>();
}

void SceneManager::onTick(double dt)
{
    if (this->currentMediator != nullptr)
    {
        this->currentMediator->onTick(dt);
    }
}

void SceneManager::onAnimate(double dt, double tickPercent)
{
    if (this->currentMediator != nullptr)
    {
        this->currentMediator->onAnimate(dt, tickPercent);
    }
}