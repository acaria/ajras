#pragma once

class InventoryPanel : public cc::Layer
{
public:
    static InventoryPanel* create(unsigned bagCapacity);
    InventoryPanel(unsigned bagCapacity);
    virtual ~InventoryPanel() {}
    virtual bool init() override;
    int getHeight();
    
private:
    unsigned bagCapacity;
    const unsigned slotCatMargin = 10;
};