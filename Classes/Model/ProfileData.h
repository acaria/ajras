#pragma once

class AnimationData;
class Dir;

class ProfileData
{
public:

    ProfileData(const std::string &path);
    ~ProfileData();
    
    AnimationData*  getDirAnimation(const Dir& orientation,
                                    const std::string& key);
    AnimationData*  getKeyAnimation(const std::string& key);
    
    //collision
    std::string     collisionCat;
    cocos2d::Rect   collisionRect;
    
    //behaviour
    std::string     behaviourKey;
    std::string     behaviourMood;
    
    //stats - move
    double          speed;
    double          acceleration;
    double          deceleration;
    bool            orientation;
    
    //stats - sight
    double          sightRange;
    
    //stats - melee
    std::string     meleeType;
    double          meleeRange;
    std::string     meleeAnimKey;
    
    unsigned        health;
    
    bool withCollision = false;
    bool withMove = false;
    bool withHealth = false;
    bool withSight = false;
    bool withMelee = false;
    bool withBehaviour = false;
    
    static std::string getTagName(const Dir& orientation);
    bool animCategoryExists(const std::string& category);
    
    std::string toString();
    
private:
    std::string path;

    //animations
    std::map<std::string, AnimationData*> animationData;
    std::vector<std::string> animationCategory;
    void extractAnims(const std::string& rootKey,
                      const cocos2d::ValueMap& data);
};