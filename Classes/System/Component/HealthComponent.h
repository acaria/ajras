#pragma once
#include "ProfileData.h"

struct HealthComponent
{
    void setProfile(ProfileData* profile)
    {
        assert(profile->stats != nullptr);
        assert(profile->stats->health != nullptr);
        this->set(profile->stats->health.Value,
                  profile->stats->health.Value);
    }
    
    void set(float maxHP, float HP)
    {
        this->maxHp = maxHP;
        this->hp = HP;
    }

    //input
    float maxHp;
    float hp;
    
    //output
    float damage = 0;
};