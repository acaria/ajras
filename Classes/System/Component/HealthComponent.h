#pragma once
#include "ProfileData.h"

struct HealthComponent
{
    void setProfile(ProfileData* profile)
    {
        assert(profile->stats != nullptr);
        assert(profile->stats.Value.health != nullptr);
        this->set(profile->stats.Value.health.Value,
                  profile->stats.Value.health.Value);
    }
    
    void set(float maxHP, float HP)
    {
        this->maxHp = maxHP;
        this->hp = HP;
    }

    //input
    float maxHp;
    float hp;
};