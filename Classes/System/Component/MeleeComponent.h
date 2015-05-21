#pragma once

class ProfileData;

struct MeleeComponent
{
    enum type { SELF, DIR };
    
    void setProfile(ProfileData* profile)
    {
        enum type meleeType = type::SELF;
        if (profile->meleeType == "dir")
            meleeType = type::DIR;
        this->set(profile->meleeAnimKey, meleeType, profile->meleeRange);
    }
    
    void set(const std::string& name, type meleeType, unsigned range)
    {
        this->range = range;
        this->type = meleeType;
        this->name = name;
        this->coolDownDuration = 2.f;
        this->processed = false;
        this->launched = false;
        this->curDir = 0;
    }

    double      lastTime = 0;
    unsigned    damage = 1;
    
    bool        processed;
    bool        launched;
    
    Dir curDir;
    
    //input
    std::string name;
    unsigned    range;
    type        type;
    float       coolDownDuration;
    //-----
    
    bool isCoolDown()
    {
        return !launched && (lastTime == 0 || (lib::now() - lastTime > coolDownDuration));
    }
    
    void launch(Dir dir)
    {
        curDir = dir;
        lastTime = lib::now();
        launched = true;
        processed = false;
    }
};