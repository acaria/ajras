#include "GearComponent.h"
#include "GameCtrl.h"

void GearComponent::set(std::list<ColCat> list)
{
    unsigned i = 1;
    for(auto cat : list)
    {
        auto slot = SlotData {
            .order = i++,
            .category = cat,
            .content = nullptr
        };
        
        this->slots.push_back(slot);
    }
}

bool GearComponent::checkFreeSlot(CollectibleData* data)
{
    for(auto& slot : slots)
    {
        if (slot.category == ColCat::object &&
            slot.content == nullptr)
            return true;
    }
    return false;
}

void GearComponent::addCollectible(CollectibleData* data)
{
    for(auto& slot : slots)
    {
        if (slot.category == ColCat::object &&
            slot.content == nullptr)
            slot.content = data;
    }
}

