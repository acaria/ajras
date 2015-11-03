#pragma once

#include "ProfileData.h"

struct StaminaComponent
{
    void setProfile(ProfileData* profile)
    {
        assert(profile->stats != nullptr);
        assert(profile->stats.Value.health != nullptr);
        this->max = profile->stats.Value.stamina.Value;
        this->current = this->max;
    }
    
    float max = 100.f;
    float current = 100.f;
    float recovery = 75.f;
};