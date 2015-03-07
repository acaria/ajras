#include "Headers.h"

SceneManager::SceneManager(ScreenLog &screenLog) : screenLog(&screenLog)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::go2MainMenu()
{
    this->launch<MainMenuMediator>();
}

void SceneManager::go2Game()
{
    this->launch<GameMediator>();
}