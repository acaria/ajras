#include "ProfileInteractInfo.h"

ProfileInteractInfo::ProfileInteractInfo(const cc::ValueMap& data)
{
    if (data.find("prerequisite") != data.end())
        prereqType = data.at("prerequisite").asString();
    else
        prereqType = nullptr;
    
    this->animKeys.first = data.at("on").asString();
    
    this->animKeys.second = data.at("off").asString();
    
    this->actionType = data.at("actionName").asString();
    
    if (data.find("triggerMode") != data.end())
        triggerMode = data.at("triggerMode").asString();
    else
        triggerMode = "always";
    
    if (data.find("triggerAnimation") != data.end())
        triggerAnimation = data.at("triggerAnimation").asString();
    else
        triggerAnimation = "once";
    
    if (data.find("actionParams") != data.end())
        actionParams = data.at("actionParams").asString();
    else
        actionParams = nullptr;
}