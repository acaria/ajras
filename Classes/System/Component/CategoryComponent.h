#pragma once
#include "Headers.h"

struct CategoryComponent
{
    enum class eType
    {
        NONE,
        MOOD
    };
    
    enum class eMood
    {
        NEUTRAL, HOSTILE, FRIENDLY
    };

    void setProfile(const std::string &profileName);
    void setMood(std::string& mood);
    
    eMood mood = eMood::NEUTRAL;
    
    static std::map<std::string, eType> mapType;
    static std::map<std::string, eMood> mapMood;
};