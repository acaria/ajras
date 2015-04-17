#pragma once
#include "Headers.h"

class ProfileModel
{
public:
    ~ProfileModel();
    void clear();
    ProfileData *get(const std::string &profileName);

private:
    std::map<std::string, ProfileData*> profiles;
};