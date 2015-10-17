#include "InventoryPanel.h"

bool InventoryPanel::init()
{
    if (!Node::init())
        return false;
    
    this->setCascadeOpacityEnabled(true);
    
    auto sep1 = cc::Sprite::createWithSpriteFrameName("inv_sep.png");
    sep1->setPosition({33,23});
    this->addChild(sep1);
    
    auto sep2 = cc::Sprite::createWithSpriteFrameName("inv_sep.png");
    sep2->setPosition({122,23});
    this->addChild(sep2);
    
    auto catBag = cc::Sprite::createWithSpriteFrameName("inv_cat_bag.png");
    catBag->setPosition({75,16});
    this->addChild(catBag);
    
    return true;
}

void InventoryPanel::registerPlayer(unsigned eid, const std::list<SlotData>& slotList)
{
    std::vector<Slot> resultList;
    
    cc::Point origin = {0,35};
    
    unsigned cp = 0;
    for(auto s : slotList)
    {
        cc::Sprite* slot = cc::Sprite::createWithSpriteFrameName("inv_slot.png");
        slot->setPosition({origin.x + (cp % 4) * 39.f,
                           origin.y + (int)(cp / 4) * 39.f});
        slot->setAnchorPoint({0,0});
        slot->setCascadeOpacityEnabled(true);
        this->addChild(slot);
        
        if (s.quantity == 0)
        {
            resultList.push_back({
                .quantity = 0,
                .frameName = "",
                .slot = slot
            });
        }
        else
        {
            auto sIcon = cc::Sprite::createWithSpriteFrameName(s.content->spriteFrameName);
            sIcon->setPosition({18,18});
            sIcon->setScale(26.0f / MAX(sIcon->getContentSize().width,
                                        sIcon->getContentSize().height));
            slot->addChild(sIcon);
            resultList.push_back({
                .quantity = s.quantity,
                .frameName = s.content->spriteFrameName,
                .slot = slot
            });
        }
        
        cp++;
    }
    
    slotsByEntity[eid] = resultList;
}

void InventoryPanel::updatePlayer(unsigned eid, const std::list<SlotData>& slotList)
{
    if (!lib::hasKey(slotsByEntity, eid))
        return;
    auto& resultList = slotsByEntity[eid];
    unsigned index = 0;
    for(auto& s : slotList)
    {
        
        if (s.quantity == 0)
        {
            resultList[index].quantity = 0;
            resultList[index].slot->removeAllChildren();
        }
        else
        {
            resultList[index].quantity = s.quantity;
            if (resultList[index].frameName != s.content->spriteFrameName)
            {
                resultList[index].slot->removeAllChildren();
                auto sIcon = cc::Sprite::createWithSpriteFrameName(s.content->spriteFrameName);
                sIcon->setScale(26.0f / MAX(sIcon->getContentSize().width,
                                            sIcon->getContentSize().height));
                sIcon->setPosition({18,18});
                
                resultList[index].slot->addChild(sIcon);
            }
        }
        index++;
    }
}