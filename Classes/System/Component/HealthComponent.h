#pragma once
#include "ProfileData.h"

struct HealthComponent
{
    void setProfile(ProfileData* profile)
    {
        assert(profile->stats != nullptr);
        assert(profile->stats.Value.health != nullptr);
        this->set(profile->stats.Value.health.Value.value);
    }
    
    void set(int hp)
    {
        this->maxHp = hp;
        this->hp = maxHp;
    }

    //input
    int maxHp;
    
    int hp;
};