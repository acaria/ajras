#pragma once

#include "ProfileData.h"

struct StaminaComponent
{
    void setProfile(ProfileData* profile)
    {
        assert(profile->stats != nullptr);
        assert(profile->stats->health != nullptr);
        this->max = profile->stats->stamina.Value;
        this->current = this->max;
    }
    
    float max = 100.f;
    float current = 100.f;
    float recovery = 20.f;
};