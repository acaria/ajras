#include "MainMenuMediator.h"
#include "MainMenuScene.h"
#include "GameCtrl.h"

void MainMenuMediator::onAddView(MainMenuScene& scene)
{
    this->eventRegs.push_back(scene.onMenuCmd.registerObserver(
        [&scene](MainMenuScene::CmdTag tag) {
            switch(tag)
            {
                case MainMenuScene::CmdTag::INTRO:
                    scene.goIntro();
                    break;
                case MainMenuScene::CmdTag::START:
                    GameCtrl::instance()->goToMission();
                    break;
                case MainMenuScene::CmdTag::QUIT:
                    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                    exit(0);
#endif
                    break;
                default:
                    Log("unsupported ItemTag in MainMenuMediator");
                    break;
            }
    }));
}

void MainMenuMediator::onRemoveView(MainMenuScene& scene)
{
}