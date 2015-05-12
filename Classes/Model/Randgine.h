#pragma once
#include "Random.h"
#include "Singleton.h"

class Randgine : public Singleton<Randgine>
{
public:

    const long MAX = 1000;
    
    enum CAT
    {
        MAP,
        AI,
        
        _SIZE
    };
    
    Randgine():randgine() {
        for(int i = 0; i < CAT::_SIZE; i++)
            subEngines.push_back(lib::Random((std::mt19937::result_type)randgine.interval((long)0, MAX)));
    }

    void setMaster(long seed)
    {
        CCLOG("master seed: %ld", seed);
        masterSeed = seed;
        randgine.getEngine().seed((std::mt19937::result_type)seed);
        
        for(int i = 0; i < CAT::_SIZE; i++)
        {
            long subSeed = randgine.interval((long)0, MAX);
            subEngines[i].getEngine().seed((std::mt19937::result_type)subSeed);
            CCLOG("subseed n%d: %ld", i + 1, subSeed);
        }
    }
    
    long getMasterSeed()
    {
        return masterSeed;
    }
    
    lib::Random& get(CAT category)
    {
        return subEngines[category];
    }

private:
    lib::Random randgine;
    long masterSeed = 0;
    std::vector<lib::Random> subEngines;
};