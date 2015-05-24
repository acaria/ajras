#pragma once
class ProfileData;

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
    
    enum class eSleep
    {
        NONE, BIRD, HUMAN
    };

    void setProfile(const std::string &profileName);
    void setProfile(ProfileData* profile);

    void setMood(std::string& mood);
    void setSleep(std::string& sleep);
    
    eMood mood = eMood::NEUTRAL;
    eSleep sleep = eSleep::NONE;
    
    static std::map<std::string, eType>     mapType;
    static std::map<std::string, eMood>     mapMood;
    static std::map<std::string, eSleep>    mapSleep;
};