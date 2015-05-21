#include "HealthBar.h"
#include "Misc.h"

HealthBar* HealthBar::create(const std::string& bg, const std::string& full, const std::string& empty)
{
    HealthBar* health = new (std::nothrow) HealthBar();
    if(health && health->initWithKey(bg, full, empty))
    {
        health->autorelease();
        return health;
    }
    CC_SAFE_DELETE(health);
    return nullptr;
}

bool HealthBar::initWithKey(const std::string& bg, const std::string& full, const std::string& empty)
{
    if (!this->init())
        return false;
    this->setAnchorPoint({0,0});
    
    this->fullKey = full;
    this->emptyKey = empty;
    
    this->bLeft = Sprite::createWithSpriteFrameName(bg + "-l.png");
    this->bLeft->setAnchorPoint({0,0});
    this->bMiddle = Sprite::createWithSpriteFrameName(bg + "-m.png");
    this->bMiddle->setAnchorPoint({0,0});
    this->bRight = Sprite::createWithSpriteFrameName(bg + "-r.png");
    this->bRight->setAnchorPoint({0,0});
    
    this->bMiddle->setPosition({this->bLeft->getContentSize().width,0});
    this->bRight->setPosition({this->bLeft->getContentSize().width + this->bMiddle->getContentSize().width,0});
    
    minimumWidth = this->bLeft->getContentSize().width +
                   this->bRight->getContentSize().width +
                   this->bMiddle->getContentSize().width;
    this->contentSize.width = minimumWidth;
    this->contentSize.height = bMiddle->getContentSize().height;
    
    this->addChild(bLeft);
    this->addChild(bMiddle);
    this->addChild(bRight);
    
    return true;
}

void HealthBar::initProperties(unsigned int maxHealth, unsigned int current)
{
    float length = 0;
    for(auto i = 0; i < maxHealth; i++)
    {
        cc::Sprite* full = nullptr;
        cc::Sprite* empty = nullptr;
        
        if (hearts.size() == i)
        {
            full = Sprite::createWithSpriteFrameName(this->fullKey + ".png");
     
            cc::Point pos = {
                i * (full->getContentSize().width + 3) + contentSize.height / 2 + 2,
                contentSize.height / 2
            };
        
            length += full->getContentSize().width + 3;
            full->setPosition(pos);
            this->addChild(full);
        
            empty = Sprite::createWithSpriteFrameName(this->emptyKey + ".png");
            empty->setPosition(pos);
            this->addChild(empty);
            
            hearts.push_back({full, empty});
        }
        else
        {
            full = hearts[i].first;
            empty = hearts[i].second;
        }
        
        full->setOpacity((i<current)?255:0);
        empty->setOpacity((i<current)?0:255);
    }
    
    while(hearts.size() > maxHealth)
    {
        this->removeChild(hearts.back().first, true);
        this->removeChild(hearts.back().second, true);
        hearts.pop_back();
    }
    
    this->setLength(length + contentSize.height);
}

void HealthBar::updateProperties(unsigned health)
{
    float length = 0;
    for(auto i = 0; i < hearts.size(); i++)
    {
        auto full = hearts[i].first;
        auto empty = hearts[i].second;
        
        full->setOpacity((i<health)?255:0);
        empty->setOpacity((i<health)?0:255);
    }
}

const cc::Size& HealthBar::getContentSize()
{
    return contentSize;
}

void HealthBar::setLength(float length)
{
    auto newSize = cc::Size(
        lib::clamp<double>(length, this->minimumWidth, length),
        this->bLeft->getContentSize().height
    );
    this->bMiddle->setScaleX(
        newSize.width - this->bLeft->getContentSize().width - this->bRight->getContentSize().height / this->bMiddle->getContentSize().width);
    this->bRight->cocos2d::Node::setPosition({this->bMiddle->getPosition().x + this->bMiddle->getContentSize().width * this->bMiddle->getScaleX(), 0});
    this->contentSize = newSize;
}