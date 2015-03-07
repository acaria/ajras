#include "Headers.h"

using namespace ui;

bool MainMenuScene::init()
{
    if (!Base::init())
        return false;
    
    auto bg = LayerGradient::create(Color4B(100, 100, 255, 255),
                                    Color4B(200, 200, 250, 255),
                                    Point(0, 1));
    this->addChild(bg);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    
    auto bStart = Button::create();
    bStart->setTouchEnabled(true);
    //bStart->setTitleColor(Color3B(100, 0, 0));
    bStart->setTitleFontName("fonts/NeutraText-BoldSC.otf");
    bStart->setTitleFontSize(32);
    bStart->setTitleText("Start");
    bStart->loadTextures("btn_blue_up.png", "btn_blue_down.png", "", Widget::TextureResType::PLIST);
    bStart->setScale9Enabled(true);
    bStart->setSize(Size(200, 50));
    bStart->setPosition(Point(
                              origin.x + visibleSize.width / 2,
                              origin.y + visibleSize.height / 2 + 100));
    bStart->addTouchEventListener(CC_CALLBACK_2(MainMenuScene::touchEvent, this));
    this->addChild(bStart, 0, (int)ItemTag::START);
    
    auto bOptions = Button::create();
    bOptions->setTouchEnabled(true);
    bOptions->setTitleFontName("fonts/NeutraText-BoldSC.otf");
    bOptions->setTitleFontSize(32);
    bOptions->setTitleText("Options");
    bOptions->loadTextures("btn_blue_up.png",
                           "btn_blue_down.png",
                           "",
                           Widget::TextureResType::PLIST);
    bOptions->setScale9Enabled(true);
    bOptions->setSize(Size(200, 50));
    bOptions->setPosition(Point(
                                origin.x + visibleSize.width / 2,
                                origin.y + visibleSize.height / 2));
    bOptions->addTouchEventListener(CC_CALLBACK_2(MainMenuScene::touchEvent, this));
    this->addChild(bOptions, 0, (int)ItemTag::OPTIONS);
    
    auto bCredits = Button::create();
    bCredits->setTouchEnabled(true);
    bCredits->setTitleFontName("fonts/NeutraText-BoldSC.otf");
    bCredits->setTitleFontSize(32);
    bCredits->setTitleText("Credits");
    bCredits->loadTextures("btn_blue_up.png", "btn_blue_down.png", "", Widget::TextureResType::PLIST);
    bCredits->setScale9Enabled(true);
    bCredits->setSize(Size(200, 50));
    bCredits->setPosition(Point(
                                origin.x + visibleSize.width / 2,
                                origin.y + visibleSize.height / 2 - 100));
    bCredits->addTouchEventListener(CC_CALLBACK_2(MainMenuScene::touchEvent, this));
    this->addChild(bCredits, 0 , (int)ItemTag::CREDITS);
    
    auto bQuit = Button::create();
    bQuit->setTouchEnabled(true);
    bQuit->setTitleFontName("fonts/NeutraText-BoldSC.otf");
    bQuit->setTitleFontSize(32);
    bQuit->setTitleText("Exit");
    bQuit->loadTextures("btn_blue_up.png", "btn_blue_down.png", "", Widget::TextureResType::PLIST);
    bQuit->setScale9Enabled(true);
    bQuit->setSize(Size(200, 50));
    bQuit->setPosition(Point(
                             origin.x + visibleSize.width / 2,
                             origin.y + visibleSize.height / 2 - 200));
    bQuit->addTouchEventListener(CC_CALLBACK_2(MainMenuScene::touchEvent, this));
    this->addChild(bQuit, 0 , (int)ItemTag::QUIT);
    
    auto copyInfo = ui::Text::create();
    copyInfo->setString("test font - 2015");
    copyInfo->setFontName("fonts/Outage-Cut.ttf");
    copyInfo->setFontSize(14);
    copyInfo->setAnchorPoint(Point(0.5, 0));
    copyInfo->cocos2d::Node::setPosition(Point(
                                               origin.x + visibleSize.width / 2,
                                               origin.y + 5));
    
    this->addChild(copyInfo);
    
    return true;
}

void MainMenuScene::touchEvent(Ref *pSender, Widget::TouchEventType type)
{
    auto currentItem = static_cast<Button*>(pSender);
    if (!currentItem)
        return;
    switch (type)
    {
        case Widget::TouchEventType::ENDED:
            onMenuSelect(static_cast<ItemTag>(currentItem->getTag()));
            break;
        default:
            break;
    }
}
