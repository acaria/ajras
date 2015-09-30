#include "GearComponent.h"
#include "GameCtrl.h"

void GearComponent::set(const std::list<SlotData*>& list)
{
    this->slots = list;
}

bool GearComponent::checkFreeSlot(const CollectibleData* data)
{
    for(auto& slot : slots)
    {
        if (slot->category == ColCat::object &&
            slot->content == nullptr)
            return true;
    }
    return false;
}

void GearComponent::addCollectible(CollectibleData* data)
{
    for(auto& slot : slots)
    {
        if (slot->category == ColCat::object &&
            slot->content == nullptr)
        {
            slot->content = data;
            break;
        }
    }
}

