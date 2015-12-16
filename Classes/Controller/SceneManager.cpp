#include "SceneManager.h"
#include "MainMenuMediator.h"
#include "MissionMediator.h"
#include "CampMediator.h"
#include "TestMediator.h"
#include "TestScene.h"

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

void SceneManager::go2Test()
{
    this->launch<TestMediator<TestScene>>();
}

BaseMediator* SceneManager::getCurMediator()
{
    return currentMediator;
}