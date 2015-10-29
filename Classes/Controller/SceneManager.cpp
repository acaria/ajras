#include "SceneManager.h"
#include "MainMenuMediator.h"
#include "MissionMediator.h"
#include "CampMediator.h"

SceneManager::SceneManager(TickCtrl* tickCtrl) : tickCtrl(tickCtrl)
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

void SceneManager::go2Camp()
{
    this->launch<CampMediator>();
}

BaseMediator* SceneManager::getCurMediator()
{
    return currentMediator;
}

EventScene* SceneManager::getCurScene()
{
    return currentScene;
}