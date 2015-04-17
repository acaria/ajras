#include "CategoryComponent.h"
#include "GameCtrl.h"
#include "ProfileData.h"

void CategoryComponent::setProfile(const std::string &profileName)
{
    this->setProfile(GameCtrl::instance()->profileModel.get(profileName));
}

void CategoryComponent::setProfile(ProfileData* profile)
{
    this->setMood(profile->behaviourMood);
}

void CategoryComponent::setMood(std::string& mood)
{
    this->mood = this->mapMood[mood];
}

std::map<std::string, CategoryComponent::eMood> CategoryComponent::mapMood = {
    {"neutral", eMood::NEUTRAL},
    {"hostile", eMood::HOSTILE},
    {"friendly",eMood::FRIENDLY}
};

std::map<std::string, CategoryComponent::eType> CategoryComponent::mapType = {
    {"mood", eType::MOOD},
    {"none", eType::NONE}
};
