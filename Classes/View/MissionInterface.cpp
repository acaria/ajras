#include "MissionInterface.h"
#include "Dir.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

StickControl* MissionInterface::getStick()
{
    return this->stick;
}

void MissionInterface::addPlayerEntity(unsigned int eid, const std::string& profileName,
                                       float maxHealth, float maxStamina)
{
    auto status = cc::create<EntityStatus>(maxHealth, maxStamina);
    status->cocos2d::Node::setAnchorPoint({0,1});
    status->setPosition(this->entityStatusPosition);
    
    cc::Sprite* picto = nullptr;
    
    if (profileName == "boy")
        picto = cc::Sprite::createWithSpriteFrameName("entity_icon_crown.png");
    else if (profileName == "boy2")
        picto = cc::Sprite::createWithSpriteFrameName("entity_icon_shield.png");
        
    if (picto != nullptr)
    {
        picto->setAnchorPoint({0,0});
        picto->setPosition({7,3});
        status->addChild(picto);
    }
    
    this->addChild(status);
    entityStatusMap[eid] = status;
    entityStatusPosition.y -= 44;
}

void MissionInterface::setInventoryList(SlotCurrency currency, std::list<SlotData> &list)
{
    this->currency = currency;
    unsigned gold = (currency.stackability == -1) ? currency.quantity :
        MIN(currency.quantity, currency.stackability);
    this->txtGoldAmount->setString(std::to_string(gold));
    this->inventoryPanel->fill(list);
    auto newHeight = this->inventoryPanel->getHeight();
    this->actionPanel->setContentSize({def::canvasMissionMargin.x - 15, newHeight + 8 + 80});
    this->actionTitle->setPosition({5, newHeight + 18});
}

void MissionInterface::updateInventorySlot(SlotData* slot)
{
    this->inventoryPanel->update(slot);
}

void MissionInterface::updateCurrencySlot(unsigned quantity)
{
    currency.quantity += quantity;
    if (currency.stackability != -1) //not unlimited
    {
        currency.quantity = MIN(currency.stackability, currency.quantity);
    }
    this->txtGoldAmount->setString(std::to_string(currency.quantity));
}

bool MissionInterface::init()
{
    if (!Node::init())
        return false;
    
    auto pOrigin = cc::Director::getInstance()->getVisibleOrigin();
    auto pSize = cc::Director::getInstance()->getVisibleSize();
    cc::Rect canvasRect = {pOrigin.x + def::canvasMissionMargin.x,
                           pOrigin.y + def::canvasMissionMargin.w,
                           pSize.width - def::canvasMissionMargin.x - def::canvasMissionMargin.y,
                           pSize.height - def::canvasMissionMargin.w - def::canvasMissionMargin.z};
    
    this->entityStatusPosition.y = pOrigin.y + pSize.height - 80;
    
    auto fBorders = cc::ui::Scale9Sprite::createWithSpriteFrameName("main_frame.png");
    fBorders->setAnchorPoint({0,0});
    fBorders->setPosition(canvasRect.origin - cc::Point(6.0f,6.0f));
    fBorders->setContentSize(canvasRect.size + cc::Size(12.0f,12.0f));
    this->addChild(fBorders);

    this->actionPanel = cc::ui::Scale9Sprite::createWithSpriteFrameName("inv_panel.png");
    this->actionPanel->setAnchorPoint({0,0});
    this->actionPanel->setPosition(6,260);
    this->actionPanel->setContentSize({def::canvasMissionMargin.x - 15, 200});
    this->addChild(this->actionPanel);

    this->actionTitle = cc::Node::create();
    this->actionTitle->setPosition({5, this->actionPanel->getContentSize().height - 5});
    
    auto invGold = cc::Sprite::createWithSpriteFrameName("inv_gold.png");
    invGold->setAnchorPoint({1,1});
    invGold->setPosition({155,55});

    this->txtGoldAmount = cc::Label::createWithTTF("0", def::font::neutra, 18);
    this->txtGoldAmount->setColor(cc::Color3B::YELLOW);
    this->txtGoldAmount->setAnchorPoint({1,0.5});
    this->txtGoldAmount->setPosition({135,47});

    auto invTopic = cc::Sprite::createWithSpriteFrameName("button_panel_n.png");
    invTopic->setAnchorPoint({0,0});
    invTopic->setPosition({0,5});
    
    auto invPicto = cc::Sprite::createWithSpriteFrameName("inv_cat_bag.png");
    invPicto->setAnchorPoint({0,0});
    invPicto->setPosition({8, 11});
    invTopic->addChild(invPicto);
    
    auto separator = cc::ui::Scale9Sprite::createWithSpriteFrameName("inv_sep.png");
    separator->setPosition(82, 0);
    separator->setContentSize({140,6});
    
    this->actionTitle->addChild(invGold);
    this->actionTitle->addChild(txtGoldAmount);
    this->actionTitle->addChild(invTopic);
    this->actionTitle->addChild(separator);
    
    this->inventoryPanel = cc::create<InventoryPanel>(4);
    this->inventoryPanel->setAnchorPoint({0,0});
    this->inventoryPanel->setPosition({8.f, 8.f});
    this->actionPanel->addChild(inventoryPanel);
    this->actionPanel->addChild(actionTitle);
    
    this->stick = cc::create<StickControl>("joy2.png", "joy1.png", 90, 30);
    this->stick->setPosition(pOrigin.x + 90, pOrigin.y + 90);
    this->addChild(this->stick);
    
    auto bHome = cc::ui::Button::create("button_home_n.png", "button_home_h.png",
                                        "button_home_n.png", cc::ui::Widget::TextureResType::PLIST);
    bHome->setAnchorPoint({0, 1});
    bHome->setPosition({pOrigin.x + 5, pOrigin.y + pSize.height - 5});
    this->addChild(bHome);
    
    //keyboard
    auto kListener = cc::EventListenerKeyboard::create();
    kListener->onKeyPressed = [this](KeyCode code, cocos2d::Event *event) {
        this->onKeyPressAction(code);
    };
    
    kListener->onKeyReleased = [this](KeyCode code, cocos2d::Event *event) {
        this->onKeyReleaseAction(code);
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(kListener, this);
    
    return true;
}

