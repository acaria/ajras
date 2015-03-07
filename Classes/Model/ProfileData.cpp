#include "Headers.h"

ProfileData::~ProfileData()
{
    for(auto anim : this->animationData)
    {
        delete this->animationData[anim.first];
    }
}

AnimationData* ProfileData::getDirAnimation(unsigned int orientation, bool moving)
{
    if (orientation == Dir::kNone)
        Log("getDirAnimation: bad orientation=none");
    
    auto key = (moving ? "move" : "idle") + getTagName(orientation);
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

std::string ProfileData::getTagName(unsigned int orientation)
{
    if ((orientation & Dir::kLeft) == Dir::kLeft)
        return "L";
    if ((orientation & Dir::kRight) == Dir::kRight)
        return "R";
    if ((orientation & Dir::kUp) == Dir::kUp)
        return "U";
    if ((orientation & Dir::kDown) == Dir::kDown)
        return "D";
    return "";
}

ProfileData::ProfileData(const std::string &path)
{
    auto rawData = cocos2d::FileUtils::getInstance()->getValueMapFromFile(path);

    if (rawData.find("animations") != rawData.end())
    {
        auto aData = rawData.at("animations").asValueMap();
        for(auto anim : aData)
        {
            this->animationData[anim.first] = new AnimationData(anim.first, anim.second.asValueMap());
        }
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