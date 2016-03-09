#pragma once

class EntityStatus : public cc::Sprite
{
public:
    virtual ~EntityStatus() {};
    
    virtual bool init(float maxHealth, float maxStamina);
    
    void set(float curHealth, float curStamina);
    void setHealth(float current);
    void setStamina(float current);

private:
    const float barHealthHeight = 13;
    const float barStaminaHeight = 4;
    const float barWidth = 115;

    float maxHealth;
    float maxStamina;
    float currentHealth;
    float currentStamina;
    
    cc::ui::Scale9Sprite* healthBar;
    cc::ui::Scale9Sprite* staminaBar;
};

