#include "Headers.h"

void MainMenuMediator::onAddView(MainMenuScene& scene)
{
    this->eventRegs.push_back(scene.onMenuSelect.registerObserver(
        [](MainMenuScene::ItemTag tag) {
            switch(tag)
            {
                case MainMenuScene::ItemTag::START:
                    GameCtrl::instance()->newSession();
                    break;
                case MainMenuScene::ItemTag::QUIT:
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