#include "InventoryPanel.h"

bool InventoryPanel::init(unsigned nbColumns)
{
    if (!Node::init())
        return false;
    
    this->nbColumns = nbColumns;
    this->setCascadeOpacityEnabled(true);
    
    return true;
}

void InventoryPanel::update(SlotData *slot)
{
    assert(slots.find(slot) != slots.end());
    
    auto block = slots[slot];
    block->removeAllChildren();
    
    if (slot->quantity > 0)
    {
        auto sIcon = cc::Sprite::createWithSpriteFrameName(slot->content->spriteFrameName);
        sIcon->setScale(24.0f / MAX(sIcon->getContentSize().width,
                                    sIcon->getContentSize().height));
        sIcon->setPosition({20,20});
        block->addChild(sIcon);
    }
}

void InventoryPanel::fill(std::list<SlotData>& slotList)
{
    this->slots.clear();
    
    float height = (int)(slotList.size() / nbColumns) * 46;
    unsigned cp = 0;
    for(auto& s : slotList)
    {
        cc::Sprite* block = cc::Sprite::createWithSpriteFrameName("inv_slot.png");
        block->setAnchorPoint({0,0});
        block->setPosition({(cp % nbColumns) * 40.f,
                            height - (int)(cp / nbColumns) * 46.f});
        block->setCascadeOpacityEnabled(true);
        this->addChild(block);
        
        if (s.quantity > 0 && !s.content->currency)
        {
            auto sIcon = cc::Sprite::createWithSpriteFrameName(s.content->spriteFrameName);
            sIcon->setScale(26.0f / MAX(sIcon->getContentSize().width,
                                        sIcon->getContentSize().height));
            sIcon->setAnchorPoint({0,0});
            sIcon->setPosition({2,2});
            block->addChild(sIcon);
        }
        
        this->slots[&s] = block;
        cp++;
    }
}

float InventoryPanel::getHeight()
{
    return ((int)(slots.size() / nbColumns) + 1) * 46.0;
}