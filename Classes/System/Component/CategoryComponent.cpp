#include "CategoryComponent.h"
#include "GameCtrl.h"
#include "ProfileData.h"

void CategoryComponent::setProfile(const std::string &profileName)
{
    this->setProfile(GameCtrl::instance()->profileModel.get(profileName));
}

void CategoryComponent::setProfile(ProfileData* profile)
{
    this->setMood(profile->moodCategory);
    this->setSleep(profile->sleepCategory);
}

void CategoryComponent::setMood(std::string& mood)
{
    this->mood = this->mapMood[mood];
}

void CategoryComponent::setSleep(std::string& sleep)
{
    this->sleep = this->mapSleep[sleep];
}

std::map<std::string, CategoryComponent::eMood> CategoryComponent::mapMood = {
    {"neutral", eMood::NEUTRAL},
    {"hostile", eMood::HOSTILE},
    {"friendly",eMood::FRIENDLY}
};

std::map<std::string, CategoryComponent::eSleep> CategoryComponent::mapSleep = {
    {"none", eSleep::NONE},
    {"bird", eSleep::BIRD},
    {"human", eSleep::HUMAN}
};

std::map<std::string, CategoryComponent::eType> CategoryComponent::mapType = {
    {"none", eType::NONE},
    {"mood", eType::MOOD}
};
