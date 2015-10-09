#include "MainMenuScene.h"

void MainMenuScene::goIntro()
{
    this->getEventDispatcher()->removeAllEventListeners();
    this->onMenuCmd(CmdTag::START);
}

bool MainMenuScene::init()
{
    if (!Base::init())
        return false;
    
    this->setColor(cc::Color3B::BLACK);
    
    cc::Size visibleSize = cc::Director::getInstance()->getVisibleSize();
    cc::Point origin = cc::Director::getInstance()->getVisibleOrigin();
    
    auto imgLogo = cc::Sprite::createWithSpriteFrameName("logo.png");
    imgLogo->setPosition({
        origin.x + visibleSize.width / 2 - 20,
        origin.y + visibleSize.height / 2 + 100
    });
    this->addChild(imgLogo);
    
    auto imgLogo2 = cc::Sprite::createWithSpriteFrameName("line.png");
    imgLogo2->setPosition({
        origin.x + visibleSize.width / 2 + 20,
        origin.y + visibleSize.height / 2 - 20
    });
    this->addChild(imgLogo2);
    
    auto imgTap = cc::Sprite::createWithSpriteFrameName("taptostart.png");
    imgTap->setPosition({
        origin.x + visibleSize.width / 2 + 20,
        origin.y + visibleSize.height / 2 + 20
    });
    this->addChild(imgTap);
    
    auto listener = cc::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [](cc::Touch* touch, cc::Event* event){
        return true;
    };
    
    listener->onTouchEnded = [this](cc::Touch* touch, cc::Event* event){
        this->onMenuCmd(CmdTag::INTRO);
        return true;
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    
    //this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    /*auto bStart = cc::ui::Button::create();
    bStart->setTouchEnabled(true);
    bStart->setTitleFontName("fonts/NeutraText-BoldSC.otf");
    bStart->setTitleFontSize(32);
    bStart->setTitleText("Start");
    bStart->loadTextures("btn_blue_up.png", "btn_blue_down.png", "", cc::ui::Widget::TextureResType::PLIST);
    bStart->setScale9Enabled(true);
    bStart->setSize(cc::Size(200, 50));
    bStart->setPosition(cc::Point(
        origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2 + 100));
    bStart->addTouchEventListener(CC_CALLBACK_2(MainMenuScene::touchEvent, this));
    this->addChild(bStart, 0, (int)ItemTag::START);
    
    auto bOptions = cc::ui::Button::create();
    bOptions->setTouchEnabled(true);
    bOptions->setTitleFontName("fonts/NeutraText-BoldSC.otf");
    bOptions->setTitleFontSize(32);
    bOptions->setTitleText("Options");
    bOptions->loadTextures("btn_blue_up.png",
                           "btn_blue_down.png",
                           "",
                           cc::ui::Widget::TextureResType::PLIST);
    bOptions->setScale9Enabled(true);
    bOptions->setSize(cc::Size(200, 50));
    bOptions->setPosition(cc::Point(
                                origin.x + visibleSize.width / 2,
                                origin.y + visibleSize.height / 2));
    bOptions->addTouchEventListener(CC_CALLBACK_2(MainMenuScene::touchEvent, this));
    this->addChild(bOptions, 0, (int)ItemTag::OPTIONS);
    
    auto bCredits = cc::ui::Button::create();
    bCredits->setTouchEnabled(true);
    bCredits->setTitleFontName("fonts/NeutraText-BoldSC.otf");
    bCredits->setTitleFontSize(32);
    bCredits->setTitleText("Credits");
    bCredits->loadTextures("btn_blue_up.png", "btn_blue_down.png", "", cc::ui::Widget::TextureResType::PLIST);
    bCredits->setScale9Enabled(true);
    bCredits->setSize(cc::Size(200, 50));
    bCredits->setPosition(cc::Point(
                                origin.x + visibleSize.width / 2,
                                origin.y + visibleSize.height / 2 - 100));
    bCredits->addTouchEventListener(CC_CALLBACK_2(MainMenuScene::touchEvent, this));
    this->addChild(bCredits, 0 , (int)ItemTag::CREDITS);
    
    auto bQuit = cc::ui::Button::create();
    bQuit->setTouchEnabled(true);
    bQuit->setTitleFontName("fonts/NeutraText-BoldSC.otf");
    bQuit->setTitleFontSize(32);
    bQuit->setTitleText("Exit");
    bQuit->loadTextures("btn_blue_up.png", "btn_blue_down.png", "", cc::ui::Widget::TextureResType::PLIST);
    bQuit->setScale9Enabled(true);
    bQuit->setSize(cc::Size(200, 50));
    bQuit->setPosition(cc::Point(
                             origin.x + visibleSize.width / 2,
                             origin.y + visibleSize.height / 2 - 200));
    bQuit->addTouchEventListener(CC_CALLBACK_2(MainMenuScene::touchEvent, this));
    this->addChild(bQuit, 0 , (int)ItemTag::QUIT);
    
    auto copyInfo = cc::ui::Text::create();
    copyInfo->setString("test font - 2015");
    copyInfo->setFontName("fonts/Outage-Cut.ttf");
    copyInfo->setFontSize(14);
    copyInfo->setAnchorPoint(cc::Point(0.5, 0));
    copyInfo->cocos2d::Node::setPosition(cc::Point(
                                               origin.x + visibleSize.width / 2,
                                               origin.y + 5));
    
    this->addChild(copyInfo);
    */
    
    return true;
}