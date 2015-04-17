#pragma once
class ProfileData;

class ProfileModel
{
public:
    ~ProfileModel();
    void clear();
    ProfileData *get(const std::string &profileName);

private:
    std::map<std::string, ProfileData*> profiles;
};