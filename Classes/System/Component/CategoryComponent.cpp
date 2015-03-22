#include "Headers.h"

void CategoryComponent::setProfile(const std::string &profileName)
{
    auto profile = GameCtrl::instance()->profileModel.get(profileName);
    this->setMood(profile->behaviourMood);
}

void CategoryComponent::setMood(std::string& mood)
{
    this->mood = this->mapMood[mood];
}