#include "ProfileInteractInfo.h"

ProfileInteractInfo::ProfileInteractInfo(const cc::ValueMap& data)
{
    this->prereqType = prereqTypeMap[data.at("prerequisite").asString()];
    this->animKeys.first = data.at("on").asString();
    this->animKeys.second = data.at("off").asString();
    this->actionType = actionTypeMap[data.at("actionName").asString()];
    triggerMode = triggerModeMap[data.at("triggerMode").asString()];
    if (data.find("actionParams") != data.end())
        actionParams = data.at("actionParams").asString();
    else
        actionParams = nullptr;
}

std::map<std::string, ProfileInteractInfo::TriggerMode> ProfileInteractInfo::triggerModeMap = {
    {"once", ProfileInteractInfo::TriggerMode::ONCE},
    {"always", ProfileInteractInfo::TriggerMode::ALWAYS}
};

std::map<std::string, ProfileInteractInfo::ActionType> ProfileInteractInfo::actionTypeMap = {
    {"none", ProfileInteractInfo::ActionType::NONE},
    {"reward", ProfileInteractInfo::ActionType::REWARD}
};

std::map<std::string, ProfileInteractInfo::PrereqType> ProfileInteractInfo::prereqTypeMap = {
    {"none", ProfileInteractInfo::PrereqType::NONE}
};