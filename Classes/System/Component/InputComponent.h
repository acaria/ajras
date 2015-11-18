#pragma once
#include "Dir.h"
#include "CoreLib.h"
#include "Defines.h"

struct InputComponent
{
    //output
    cc::Vec2    direction;
    
    //internal
    double      chrono = 0;
    
    bool isActive()
    {
        return enabled;
    }
    
    void disable(double duration)
    {
        enabled = false;
        if (chrono < duration)
            chrono = duration;
        if (!lib::hasKey(predicates, "time"))
        {
            predicates["time"] = [this](unsigned id){
                return chrono > 0;
            };
        }
    }
    
    void disable(const std::string& name, const std::function<bool(unsigned)>& predicate)
    {
        predicates[name] = predicate;
    }
    
    void forceDisable()
    {
        enabled = false;
        predicates["force"] = [](unsigned eid){ return true; };
    }
    
    void forceEnable()
    {
        enabled = true;
        predicates.clear();
    }
    
    void updatePredicates(unsigned eid, double dt)
    {
        chrono -= dt;
        if (chrono < 0) chrono = 0;
        
        auto itr = predicates.begin();
        while(itr != predicates.end())
        {
            if (itr->second(eid))
            {
                enabled = false;
                return;
            }
            predicates.erase(itr++);
        }
        enabled = true;
    }
    
private:
    bool enabled = true;
    std::map<std::string, std::function<bool(unsigned)>> predicates;
};