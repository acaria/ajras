#pragma once

class ProfileData;

struct MeleeComponent
{
    enum type { NONE, SELF, CHARGE, DIR };
    
    void setProfile(ProfileData* profile)
    {
        assert(profile->stats != nullptr);
        assert(profile->stats.Value.melee != nullptr);
        auto melee = profile->stats.Value.melee.Value;
        enum type meleeType = type::NONE;
        switch(lib::hash(melee.type))
        {
            case lib::hash("dir"): meleeType = type::DIR;
                break;
            case lib::hash("self"): meleeType = type::SELF;
                break;
            case lib::hash("charge"): meleeType = type::CHARGE; enabled = false;
                break;
            default: Log("unrecognised melee type: %s", melee.type.c_str());
                break;
        }
        this->set(melee.animKey, meleeType, def::mood::fromStr(profile->moodCategory), melee.range, melee.recoil);
    }
    
    void set(const std::string& name, type meleeType, def::mood::Flags moodCat, const cc::Size& range, float recoil)
    {
        this->range = range;
        this->type = meleeType;
        this->name = name;
        this->moodCat = moodCat;
        this->recoil = recoil;
    }

    unsigned    damage = 1;
    
    bool        enabled = true;
    
    //input
    std::string      name;
    float            energie = 30.0f;
    float            recoil;
    cc::Size         range;
    type             type;
    def::mood::Flags moodCat = def::mood::Neutral;
    //-----
    
    //output
    cc::Rect    atkRect;
    //-----
};