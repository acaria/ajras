#pragma once
#include "ProfileData.h"

struct HealthComponent
{
    void setProfile(ProfileData* profile)
    {
        assert(profile->stats != nullptr);
        assert(profile->stats.Value.health != nullptr);
        this->set(profile->stats.Value.health.Value.value,
                  profile->stats.Value.health.Value.value);
    }
    
    void set(int maxHP, int HP)
    {
        this->maxHp = maxHP;
        this->hp = HP;
    }

    //input
    int maxHp;
    
    int hp;
};