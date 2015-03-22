#pragma once
#include "Headers.h"

struct CategoryComponent
{
    enum class eMood
    {
        NEUTRAL, HOSTILE, FRIENDLY
    };

    void setProfile(const std::string &profileName);
    void setMood(std::string& mood);
    
    eMood mood = eMood::NEUTRAL;
    
private:
    std::map<std::string, eMood> mapMood = {
        {"neutral", eMood::NEUTRAL},
        {"hostile", eMood::HOSTILE},
        {"friendly",eMood::FRIENDLY}
    };
};