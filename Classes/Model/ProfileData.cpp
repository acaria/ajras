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
    if (withMove) result << "move - ";
    if (withHealth) result << "health - ";
    if (withSight) result << "sight - ";
    if (withMelee) result << "melee - ";
    if (withBehaviour) result << "behaviour - ";
    return result.str();
}

ProfileData::ProfileData(const std::string &path) : behaviourMood("neutral")
{
    this->path = path;
    auto rawData = cocos2d::FileUtils::getInstance()->getValueMapFromFile(path);

    if (rawData.find("animations") != rawData.end())
    {
        this->extractAnims("", rawData.at("animations").asValueMap());
    }
    
    collisionCat = "walkable";
    collisionRect = {0, 0, kBlockSize, kBlockSize};
    
    if (rawData.find("collision") != rawData.end())
    {
        withCollision = true;
        auto &cData = rawData.at("collision").asValueMap();
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
    
    if (rawData.find("stats") != rawData.end())
    {
        auto &sData = rawData.at("stats").asValueMap();
        if (sData.find("move") != sData.end())
        {
            withMove = true;
            auto &mData = sData.at("move").asValueMap();
            if (mData.find("speed") != mData.end())
                this->speed = mData.at("speed").asDouble();
            if (mData.find("acceleration") != mData.end())
                this->acceleration = mData.at("acceleration").asDouble();
            if (mData.find("deceleration") != mData.end())
                this->deceleration = mData.at("deceleration").asDouble();
            if (mData.find("orientation") != mData.end())
                this->orientation = mData.at("orientation").asBool();
        }
        
        if (sData.find("sight") != sData.end())
        {
            withSight = true;
            auto &siData = sData.at("sight").asValueMap();
            if (siData.find("range") != siData.end())
                this->sightRange = siData.at("range").asDouble();
        }
        
        if (sData.find("melee") != sData.end())
        {
            withMelee = true;
            auto &mData = sData.at("melee").asValueMap();
            if (mData.find("type") != mData.end())
                this->meleeType = mData.at("type").asString();
            if (mData.find("range") != mData.end())
                this->meleeRange = mData.at("range").asDouble();
            if (mData.find("anim_key") != mData.end())
                this->meleeAnimKey = mData.at("anim_key").asString();
        }
        
        if (sData.find("health") != sData.end())
        {
            withHealth = true;
            this->health = sData.at("health").asInt();
        }
    }
    
    if (rawData.find("behaviour") != rawData.end())
    {
        withBehaviour = true;
        auto &bData = rawData.at("behaviour").asValueMap();
        if (bData.find("key") != rawData.end())
            this->behaviourKey = bData.at("key").asString();
        if (bData.find("mood") != rawData.end())
            this->behaviourMood = bData.at("mood").asString();
    }
}