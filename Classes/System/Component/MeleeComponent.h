#pragma once

class ProfileData;

struct MeleeComponent
{
    enum type { SELF, DIR };
    
    void setProfile(ProfileData* profile)
    {
        assert(profile->stats != nullptr);
        assert(profile->stats.Value.melee != nullptr);
        auto melee = profile->stats.Value.melee.Value;
        enum type meleeType = type::SELF;
        if (melee.type == "dir")
            meleeType = type::DIR;
        this->set(melee.animKey, meleeType, melee.range);
    }
    
    void set(const std::string& name, type meleeType, const cc::Size& range)
    {
        this->range = range;
        this->type = meleeType;
        this->name = name;
        this->processing = false;
    }

    unsigned    damage = 1;
    
    bool        processing;
    
    //input
    std::string name;
    float       energie = 30.0f;
    cc::Size    range;
    type        type;
    //-----
    
    //output
    cc::Rect    atkRect;
    //-----
};