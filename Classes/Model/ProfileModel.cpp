#include "ProfileModel.h"
#include "Misc.h"
#include "ProfileData.h"

ProfileData *ProfileModel::get(const std::string &profileName)
{
    if (!lib::hasKey(this->profiles, profileName))
    {
        auto path = cc::FileUtils::getInstance()->
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
    this->profiles.clear();
}

ProfileModel::~ProfileModel()
{
    
    this->clear();
}
