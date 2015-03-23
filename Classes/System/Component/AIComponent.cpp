#include "Headers.h"

void AIComponent::setProfile(const std::string &profileName)
{
    auto profile = GameCtrl::instance()->profileModel.get(profileName);
    
    if (!profile->behaviourKey.empty())
    {
        this->bref = GameCtrl::instance()->behaviourModel.get(profile->behaviourKey);
    }
    
    sightRange = profile->sightRange;
}