#include "AIComponent.h"
#include "ModelProvider.h"
#include "ProfileData.h"

void AIComponent::setProfile(const std::string &profileName)
{
    this->setProfile(ModelProvider::instance()->profile.get(profileName));
}

void AIComponent::setProfile(ProfileData* profile)
{
    if (!profile->behaviourKey.empty())
    {
        this->bref = ModelProvider::instance()->behaviour.get(profile->behaviourKey);
    }
    
    this->mood = this->mapMood[profile->moodCategory];
    this->sleep = this->mapSleep[profile->sleepCategory];
    this->sightRange = profile->sightRange;
}

std::map<std::string, AIComponent::eMood> AIComponent::mapMood = {
    {"neutral", eMood::NEUTRAL},
    {"hostile", eMood::HOSTILE},
    {"friendly",eMood::FRIENDLY}
};

std::map<std::string, AIComponent::eSleep> AIComponent::mapSleep = {
    {"none", eSleep::NONE},
    {"bird", eSleep::BIRD},
    {"human", eSleep::HUMAN}
};

std::map<std::string, AIComponent::eType> AIComponent::mapType = {
    {"none", eType::NONE},
    {"mood", eType::MOOD}
};
