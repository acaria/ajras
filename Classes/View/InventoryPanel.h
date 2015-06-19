#pragma once

#include "Defines.h"
#include "CollectibleData.h"

class InventoryPanel : public cc::Layer
{
public:
    static InventoryPanel* create(unsigned bagCapacity);
    static InventoryPanel* create();
    InventoryPanel(unsigned bagCapacity);
    virtual ~InventoryPanel() {}
    virtual bool init() override;
    int getHeight();
    void changeCapacity(unsigned bagCapacity);
    
private:
    std::vector<ColCat> slotTagToType;
    
    unsigned bagCapacity;
    const unsigned slotCatMargin = 10;
    
    std::list<std::pair<cc::Sprite*, CollectibleData*>> slotList;
};