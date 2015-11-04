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
    
    this->sleep = this->mapSleep[profile->sleepCategory];
    
    if (profile->stats != nullptr && profile->stats.Value.sight != nullptr)
        this->sightRange = profile->stats.Value.sight.Value.range;
}

std::map<std::string, AIComponent::eSleep> AIComponent::mapSleep = {
    {"none", eSleep::NONE},
    {"bird", eSleep::BIRD},
    {"human", eSleep::HUMAN}
};

std::map<std::string, AIComponent::eType> AIComponent::mapType = {
    {"none", eType::NONE},
    {"mood", eType::MOOD}
};
