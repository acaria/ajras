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
        this->set(melee.animKey, meleeType, def::mood::fromStr(profile->moodCategory), melee.range);
    }
    
    void set(const std::string& name, type meleeType, def::mood::Flags moodCat, const cc::Size& range)
    {
        this->range = range;
        this->type = meleeType;
        this->name = name;
        this->processing = false;
        this->moodCat = moodCat;
    }

    unsigned    damage = 1;
    
    bool        processing;
    
    //input
    std::string      name;
    float            energie = 30.0f;
    cc::Size         range;
    type             type;
    def::mood::Flags moodCat = def::mood::Neutral;
    //-----
    
    //output
    cc::Rect    atkRect;
    //-----
};