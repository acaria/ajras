#pragma once

#include "Defines.h"
#include "CoreLib.h"
#include "SlotData.h"

class InventoryPanel : public cc::Node
{
public:
    virtual ~InventoryPanel() {}
    virtual bool init();
    void registerPlayer(unsigned eid, const std::list<SlotData>& list);
    void updatePlayer(unsigned eid, const std::list<SlotData>& list);
    
private:
    struct Slot
    {
        unsigned quantity;
        std::string frameName;
        cc::Sprite* slot;
    };
    
    std::map<unsigned, std::vector<Slot>> slotsByEntity;
};