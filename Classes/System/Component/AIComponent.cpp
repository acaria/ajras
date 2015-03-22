#include "Headers.h"

void AIComponent::setProfile(const std::string &profileName)
{
    auto profile = GameCtrl::instance()->profileModel.get(profileName);
    
    if (!profile->behaviourKey.empty())
    {
        this->bref = GameCtrl::instance()->behaviourModel.get(profile->behaviourKey);
        
        board.runningNode = bref->getRootNode();
    }
    
    sightRange = profile->sightRange;
}

behaviour::BaseNode* AIComponent::getNode2Run()
{
    auto cur = board.runningNode;
    if (cur != nullptr &&
        (!lib::hasKey(board.states, cur->id) ||
         board.states[cur->id] == state::RUNNING))
    {
        return cur;
    }
    return bref->getRootNode();
}