#pragma once

#include "Defines.h"
#include "StickControl.h"
#include "HealthBar.h"
#include "EntityStatus.h"
#include "InventoryPanel.h"
#include "Event.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

class MissionInterface : public cc::Node
{
private:
    
public:
    virtual ~MissionInterface() {}
    
    virtual bool init() override;
    
    //accessors
    StickControl*   getStick();
    
    //events
    lib::Subject<void(KeyCode key)> onKeyPressAction;
    lib::Subject<void(KeyCode key)> onKeyReleaseAction;
    
    void addPlayerEntity(unsigned int eid, const std::string& profileName,
                         float maxHealth, float maxStamina);
    
    void setInventoryList(SlotCurrency currency, std::list<SlotData> &list);
    void updateInventorySlot(SlotData* slot);
    void updateCurrencySlot(unsigned quantity);
    void updateHealthStatus(unsigned eid, float amount);
    void updateStaminaStatus(unsigned eid, float amount);
    
private:
    
    cc::Point entityStatusPosition = {8,0};
    cc::ui::Scale9Sprite*           actionPanel;
    cc::Node*                       actionTitle;
    InventoryPanel*                 inventoryPanel;
    cc::Label*                      txtGoldAmount;
    SlotCurrency                    currency;
    
    StickControl* stick;
    std::map<unsigned, EntityStatus*> entityStatusMap;
};