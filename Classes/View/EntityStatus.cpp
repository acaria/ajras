#include "EntityStatus.h"

void EntityStatus::set(float curHealth, float curStamina)
{
    this->setHealth(curHealth);
    this->setStamina(curStamina);
}

void EntityStatus::setHealth(float current)
{
    if (current != this->currentHealth)
    {
        this->currentHealth = current;

        float width = current / maxHealth * barHealthHeight;
        this->healthBar->setContentSize({width, barHealthHeight});
    }
}

void EntityStatus::setStamina(float current)
{
    if (current != this->currentStamina)
    {
        this->currentStamina = current;
        
        float width = current / maxStamina * barStaminaHeight;
        this->staminaBar->setContentSize({width, barStaminaHeight});
    }
}

bool EntityStatus::init(float maxHealth, float maxStamina)
{
    if (!Sprite::initWithSpriteFrameName("entity_status.png"))
        return false;
    
    this->maxHealth = maxHealth;
    this->maxStamina = maxStamina;
    this->currentHealth = maxHealth;
    this->currentStamina = maxStamina;
    
    this->healthBar = cc::ui::Scale9Sprite::createWithSpriteFrameName("entity_health.png");
    this->healthBar->setAnchorPoint({0,0});
    this->healthBar->setPosition(40, 17);
    this->healthBar->setContentSize({barWidth, barHealthHeight});
    
    this->staminaBar = cc::ui::Scale9Sprite::createWithSpriteFrameName("entity_stamina.png");
    this->staminaBar->setAnchorPoint({0,0});
    this->staminaBar->setPosition(40, 13);
    this->staminaBar->setContentSize({barWidth, barStaminaHeight});
    
    this->addChild(healthBar);
    this->addChild(staminaBar);
    
    return true;
}