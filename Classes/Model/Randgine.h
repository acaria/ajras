#pragma once
#include "Random.h"
#include "Singleton.h"

class Randgine : public Singleton<Randgine>
{
public:

    const long MAX = 1000000;
    
    enum CAT
    {
        MAP_DATA,
        MISSION,
        CAMP,
        _SIZE
    };
    
    Randgine():mainEngine()
    {
        masterSeed = 0;
        for(int i = 0; i < CAT::_SIZE; i++)
            subEngines.push_back(lib::Random());
    }

    void setMaster(long seed)
    {
        Log("master seed: %ld", seed);
        masterSeed = seed;
        subSeeds.clear();
        mainEngine.getEngine().seed((std::mt19937::result_type)seed);
        
        for(int i = 0; i < CAT::_SIZE; i++)
        {
            long subSeed = mainEngine.interval((long)0, MAX);
            subSeeds[static_cast<CAT>(i)] = subSeed;
            subEngines[i].getEngine().seed((std::mt19937::result_type)subSeed);
            Log("subseed n%d: %ld", i + 1, subSeed);
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

    //seeds
    long                        masterSeed;
    std::map<CAT, long>         subSeeds;

    //engines
    lib::Random                 mainEngine;
    std::vector<lib::Random>    subEngines;
};