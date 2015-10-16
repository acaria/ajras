#include "SlotData.h"

bool SlotData::checkFreeSlot(const std::list<lib::Nullable<SlotData> > &inventory,
                             CollectibleData* element)
{
    for(auto slot : inventory)
    {
        if (slot == nullptr)
            return true;
        auto model = slot.get().content;
        if (model->key == element->key && slot.get().quantity < model->stackability)
            return true;
    }
    return false;
}

bool SlotData::addCollectible(std::list<lib::Nullable<SlotData> > &inventory,
                              CollectibleData* element)
{
    for(auto& slot : inventory)
    {
        if (slot == nullptr)
            continue;
        auto model = slot.Value.content;
        if (model->key == element->key && slot.Value.quantity < model->stackability)
        {
            SlotData updatedSlot = slot.Value;
            updatedSlot.quantity++;
            slot = updatedSlot;
            return true;
        }
    }
    for(auto& slot : inventory)
    {
        if (slot == nullptr)
        {
            slot = {
                .quantity = 1,
                .content = element
            };
            return true;
        }
    }
    return false;
}