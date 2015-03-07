#include "Headers.h"

ProfileData *ProfileModel::get(const std::string &profileName)
{
    if (!lib::hasKey(this->profiles, profileName))
    {
        auto path = cocos2d::FileUtils::getInstance()->
            fullPathForFilename("profiles/" + profileName + ".plist");
        profiles[profileName] = new ProfileData(path);
    }
    
    return profiles[profileName];
}

void ProfileModel::clear()
{
    for (auto i : this->profiles)
    {
        delete i.second;
    }
}

ProfileModel::~ProfileModel()
{
    
    this->profiles.clear();
}
