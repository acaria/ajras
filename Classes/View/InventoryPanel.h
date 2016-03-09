#pragma once

#include "Defines.h"
#include "CoreLib.h"
#include "SlotData.h"

class InventoryPanel : public cc::Node
{
public:
    virtual ~InventoryPanel() {}
    virtual bool init(unsigned nbColumns);
    void fill(std::list<SlotData>& list);
    void update(SlotData* slot);
    float getHeight();
    
private:
    std::map<SlotData*, cc::Sprite*> slots;
    unsigned nbColumns;
};