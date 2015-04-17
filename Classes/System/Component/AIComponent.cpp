#include "Headers.h"

void AIComponent::setProfile(const std::string &profileName)
{
    this->setProfile(GameCtrl::instance()->profileModel.get(profileName));
}

void AIComponent::setProfile(ProfileData* profile)
{
    if (!profile->behaviourKey.empty())
    {
        this->bref = GameCtrl::instance()->behaviourModel.get(profile->behaviourKey);
    }
    
    sightRange = profile->sightRange;
}