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
        this->set(melee.animKey,
                  meleeType,
                  def::mood::fromStr(profile->moodCategory),
                  melee.range,
                  melee.recoil,
                  melee.stamina,
                  melee.triggerRatio);
    }
    
    void set(const lib::Nullable<std::string>& name,
             type meleeType,
             def::mood::Flags moodCat,
             const cc::Size& range,
             const ProfileStatsInfo::Recoil& recoil,
             float stamina,
             float triggerRatio)
    {
        this->range = range;
        this->type = meleeType;
        this->animKey = name;
        this->moodCat = moodCat;
        this->recoil = recoil;
        this->triggerRatio = triggerRatio;
        this->stamina = stamina;
    }

    //todo: input it
    unsigned    damage = 1;
    
    //input
    lib::Nullable<std::string>  animKey;
    float                       stamina = 100.0f;
    ProfileStatsInfo::Recoil    recoil;
    float                       triggerRatio;
    cc::Size                    range;
    type                        type;
    def::mood::Flags            moodCat = def::mood::Neutral;
    //-----
    
    //internal
    bool        enabled = true;
    
    //output
    cc::Rect    atkRect;
    //-----
};