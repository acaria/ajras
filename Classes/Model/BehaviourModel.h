#pragma once
#include "Headers.h"

class BehaviourModel
{
public:
    ~BehaviourModel();
    void clear();
    BehaviourData* get(const std::string &behaviourName);
    
private:
    std::map<std::string, BehaviourData*> behaviours;
};