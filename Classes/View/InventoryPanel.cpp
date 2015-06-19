#include "InventoryPanel.h"

InventoryPanel::InventoryPanel(unsigned bagCapacity) : bagCapacity(bagCapacity)
{
    this->setCascadeOpacityEnabled(true);
    
    slotTagToType.push_back(ColCat::object);
    slotTagToType.push_back(ColCat::head);
    slotTagToType.push_back(ColCat::body);
    slotTagToType.push_back(ColCat::leg);
    slotTagToType.push_back(ColCat::feet);
    slotTagToType.push_back(ColCat::ring);
}

InventoryPanel* InventoryPanel::create()
{
    return InventoryPanel::create(7);
}

InventoryPanel* InventoryPanel::create(unsigned bagCapacity)
{
    InventoryPanel * layer = new (std::nothrow) InventoryPanel(bagCapacity);
    if(layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void InventoryPanel::changeCapacity(unsigned int bagCapacity)
{
    //TODO
    //assert(bagCapacity >= this->bagCapacity);
    //if (bagCapacity == this->bagCapacity)
    //    return; //skip
    //
    //this->removeChildByTag(bagSlotTag, true);
}

int InventoryPanel::getHeight()
{
    return (((this->bagCapacity + 1) / 4) + 1) * 39 + slotCatMargin + 39 * 2;
}

bool InventoryPanel::init()
{
    int nbLines = (this->bagCapacity + 1) / 4;
    cc::Point origin = {0.f, (float)getHeight()};
 
    //bag
    for(int j = nbLines; j >= 0; j--)
    for(unsigned i = 0; i < 4; i++)
    {
        cc::Sprite* slot;
        if (i == 0 && j == 0) //display category
            slot = cc::Sprite::createWithSpriteFrameName("inv_cat_bag.png");
        else //display slot
        {
            slot = cc::Sprite::createWithSpriteFrameName("inv_slot_empty.png");
            slotList.push_back({slot, nullptr});
        }
        slot->setPosition({origin.x + i * 39.f, origin.y + j * - 39.f});
        slot->setAnchorPoint({0,0});
        slot->setTag(0);
        this->addChild(slot);
    }
    
    origin.y -= slotCatMargin + (nbLines + 1) * 39.f;
    
    //equipments
    for(int j = 1; j >= 0; j--)
    for(unsigned i = 0; i < 4; i++)
    {
        if (i == 0 && j == 1) //skip
            continue;
        cc::Sprite* slot;
        if (i == 0 && j == 0) //display category
            slot = cc::Sprite::createWithSpriteFrameName("inv_cat_eqp.png");
        else //display slot
        {
            slot = cc::Sprite::createWithSpriteFrameName("inv_slot_eqp" + std::to_string(i + j * 4) + ".png");
            slot->setTag(i + j * 4);
            slotList.push_back({slot, nullptr});
        }
        slot->setPosition({origin.x + i * 39.f, origin.y + j * - 39.f});
        slot->setAnchorPoint({0,0});
        this->addChild(slot);
    }

    return true;
}