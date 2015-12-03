#include "AIComponent.h"
#include "ModelProvider.h"
#include "ProfileData.h"

void AIComponent::setProfile(const std::string &profileName)
{
    this->setProfile(ModelProvider::instance()->profile.get(profileName));
}

void AIComponent::setProfile(ProfileData* profile)
{
    assert(profile->behaviour != nullptr);
    this->bref = ModelProvider::instance()->behaviour.get(profile->behaviour->key);
    this->sleep = this->mapSleep[profile->behaviour->sleepCategory];
    this->sightRange = profile->behaviour->sightRange;
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
