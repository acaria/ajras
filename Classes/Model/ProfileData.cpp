#include "ProfileData.h"
#include "AnimationData.h"
#include "Dir.h"
#include "Defines.h"

ProfileData::~ProfileData()
{
    for(auto anim : this->animationData)
    {
        delete this->animationData[anim.first];
    }
}

AnimationData* ProfileData::getDirAnimation(const Dir &orientation,
                                            const std::string& pre)
{
    if (orientation == Dir::None)
    {
        if (lib::hasKey(this->animationData, pre))
            return this->animationData[pre];
        Log("getDirAnimation: bad key=%s", pre.c_str());
    }
    else
    {
        auto key = pre + getTagName(orientation);
        if (lib::hasKey(this->animationData, key))
            return this->animationData[key];
        Log("getDirAnimation: bad key=%s", key.c_str());
    }
    
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

std::string ProfileData::getTagName(const Dir &orientation)
{
    if (orientation.contains(Dir::Left))
        return "_left";
    if (orientation.contains(Dir::Right))
        return "_right";
    if (orientation.contains(Dir::Up))
        return "_up";
    if (orientation.contains(Dir::Down))
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

std::string ProfileData::toString()
{
    std::stringstream result;
    
    result << "profile: " << this->path << std::endl;
    if (withCollision) result << "collision - ";
    if (withBehaviour) result << "behaviour - ";
    return result.str();
}

ProfileData::ProfileData(const std::string &path) : moodCategory("neutral")
{
    this->path = path;
    auto rawData = cocos2d::FileUtils::getInstance()->getValueMapFromFile(path);

    if (rawData.find("animations") != rawData.end())
    {
        this->extractAnims("", rawData.at("animations").asValueMap());
    }
    
    collisionCat = "walkable";
    collisionRect = cc::Rect::ZERO;
    
    if (rawData.find("collision") != rawData.end())
    {
        withCollision = true;
        auto &cData = rawData.at("collision").asValueMap();
        if (cData.find("rect") != cData.end())
        {
            std::vector<std::string> split;
            lib::split(cData.at("rect").asString(), split, ", ");
            this->collisionRect = cocos2d::Rect(std::stoi(split[0]),
                                                std::stoi(split[1]),
                                                std::stoi(split[2]),
                                                std::stoi(split[3]));
        }
        
        if (cData.find("cat") != cData.end())
        {
            this->collisionCat = cData.at("cat").asString();
        }
    }
    
    if (rawData.find("interaction") != rawData.end())
    {
        this->interaction = ProfileInteractInfo(rawData.at("interaction").asValueMap());
    }
    
    if (rawData.find("stats") != rawData.end())
    {
        this->stats = ProfileStatsInfo(rawData.at("stats").asValueMap());
    }
    
    if (rawData.find("behaviour") != rawData.end())
    {
        auto &bData = rawData.at("behaviour").asValueMap();
        withBehaviour = true;
        this->behaviourKey = bData["key"].asString();
        if (bData.find("sleep") != bData.end())
            this->sleepCategory = bData.at("sleep").asString();
        if (bData.find("mood") != bData.end())
            this->moodCategory = bData.at("mood").asString();
    }
}