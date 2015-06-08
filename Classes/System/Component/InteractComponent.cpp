#include "InteractComponent.h"

std::map<std::string, InteractComponent::ePrereq> InteractComponent::prereqMap = {
    {"none", InteractComponent::ePrereq::NONE}
};

std::map<std::string, InteractComponent::eAction> InteractComponent::actionMap = {
    {"none", InteractComponent::eAction::NONE},
    {"reward", InteractComponent::eAction::REWARD}
};