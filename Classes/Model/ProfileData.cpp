#include "Headers.h"

ProfileData::~ProfileData()
{
    for(auto anim : this->animationData)
    {
        delete this->animationData[anim.first];
    }
}

AnimationData* ProfileData::getDirAnimation(unsigned int orientation,
                                            const std::string& pre)
{
    if (orientation == Dir::kNone)
        Log("getDirAnimation: bad orientation=none");
    
    auto key = pre + getTagName(orientation);
    if (lib::hasKey(this->animationData, key))
        return this->animationData[key];
    Log("getDirAnimation: bad key=%s", key.c_str());
    return nullptr;
}

AnimationData* ProfileData::getKeyAnimation(const std::string &key)
{
    if (lib::hasKey(this->animationData, key))
        return this->animationData[key];
    return nullptr;
}

bool ProfileData::animCategoryExists(const std::string &category)
{
    for (auto cat : this->animationCategory)
    {
        if (cat == category)
            return true;
    }
    return false;
}

std::string ProfileData::getTagName(unsigned int orientation)
{
    if ((orientation & Dir::kLeft) == Dir::kLeft)
        return "_left";
    if ((orientation & Dir::kRight) == Dir::kRight)
        return "_right";
    if ((orientation & Dir::kUp) == Dir::kUp)
        return "_up";
    if ((orientation & Dir::kDown) == Dir::kDown)
        return "_down";
    return "";
}

void ProfileData::extractAnims(const std::string& rootKey,
                               const cocos2d::ValueMap &data)
{
    for(auto anim : data)
    {
        if (anim.first[0] == '_')
        {
            auto category = anim.first.substr(1);
            if (rootKey.empty())
                this->animationCategory.push_back(category);
            this->extractAnims(rootKey + category + "_", anim.second.asValueMap());
        }
        else
        {
            this->animationData[rootKey + anim.first] = new AnimationData(
                rootKey + anim.first,
                anim.second.asValueMap());
        }
    }
}

ProfileData::ProfileData(const std::string &path)
{
    auto rawData = cocos2d::FileUtils::getInstance()->getValueMapFromFile(path);

    if (rawData.find("animations") != rawData.end())
    {
        this->extractAnims("", rawData.at("animations").asValueMap());
    }
    
    collisionCat = "walkable";
    collisionRect = {0, 0, 16, 16};
    
    if (rawData.find("collision") != rawData.end())
    {
        auto cData = rawData.at("collision").asValueMap();
        if (cData.find("rect") != cData.end())
        {
            std::vector<std::string> split;
            lib::split(cData.at("rect").asString(), split, ", ");
            this->collisionRect = cocos2d::Rect(lib::parseInt(split[0]),
                                                lib::parseInt(split[1]),
                                                lib::parseInt(split[2]),
                                                lib::parseInt(split[3]));
        }
        
        if (cData.find("cat") != cData.end())
        {
            this->collisionCat = cData.at("cat").asString();
        }
    }
}