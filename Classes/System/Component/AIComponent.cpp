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
    this->sleep = profile->behaviour->sleepCategory;
    this->sightRange = profile->behaviour->sightRange;
}