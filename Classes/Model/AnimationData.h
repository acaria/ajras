#pragma once

#include "CoreLib.h"

struct AnimationData
{
    AnimationData(const std::string& key, const cocos2d::ValueMap &data)
        :key(key)
    {
        this->flipX = false;
        this->flipY = false;
        
        if (data.find("delay") != data.end())
            this->delay = data.at("delay").asFloat();
        
        if (data.find("flipX") != data.end())
            this->flipX = data.at("flipX").asBool();
        
        if (data.find("flipY") != data.end())
            this->flipY = data.at("flipY").asBool();
        
        unsigned frameCount = 0;
        if (data.find("frameCount") != data.end())
            frameCount = data.at("frameCount").asInt();
        
        if (frameCount <= 1)
            this->frameNames.push_back(data.at("format").asString());
        else
        {
            if (data.find("customFormat") != data.end())
            {
                auto frames = data.at("customFormat").asValueVector();
                for(auto frame : frames)
                    this->frameNames.push_back(frame.asString());
            }
            else //format
            {
                auto format = data.at("format").asString();
                if (data.find("frameOrder") != data.end()) //custom order
                {
                    std::list<std::string> orderList;
                    lib::split(data.at("frameOrder").asString(), orderList, " ,");
                    
                    for(auto order : orderList)
                        this->frameNames.push_back(lib::format(format, std::stoi(order)));
                }
                else
                {
                    for(int i = 1; i <= frameCount; i++)
                        this->frameNames.push_back(lib::format(format, i));
                }
            }
        }
    }
    
    float                       delay;
    std::vector<std::string>    frameNames;
    std::string                 key;
    bool                        flipX;
    bool                        flipY;
    
    unsigned frameCount()
    {
        return frameNames.size();
    }
    
    float duration()
    {
        return this->frameCount() * this->delay;
    }
};