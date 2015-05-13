#pragma once
#include "Dir.h"
#include "Misc.h"
#include "Defines.h"

struct InputComponent
{
    //output
    bool        disabled = false;
    Dir         orientation;
    Dir         lastOrientation;
    cc::Vec2    direction;
    ActionMode  actionMode = ActionMode::walk;
    
    void disable(float duration)
    {
        double lastTime = lib::now();
        predicates["time"] = [duration, lastTime](unsigned id){
            return lib::now() - lastTime > duration;
        };
    }
    
    void forceDisable()
    {
        disabled = true;
        predicates["force"] = [](unsigned id) {
            return false;
        };
    }
    
    bool checkPredicates(unsigned eid)
    {
        if (predicates.size() == 0)
            return true;
        
        auto itr = predicates.begin();
        while(itr != predicates.end())
        {
            if (!itr->second(eid))
            {
                disabled = true;
                return false;
            }
            predicates.erase(itr++);
        }
        disabled = false;
        return true;
    }
    
    void forceEnable()
    {
        disabled = false;
        predicates.erase("force");
    }
    
    void setDirection(Dir orientation)
    {
        this->lastOrientation = this->orientation;
        this->orientation = orientation;
        this->direction = orientation.toVec();
    }
    
    void setDirection(cc::Vec2 direction)
    {
        this->lastOrientation = this->orientation;
        this->orientation = Dir::fromVec(direction);
        if (direction.getLength() > 1.0)
            this->direction = direction.getNormalized();
        else
            this->direction = direction;
    }
    
private:
    std::map<std::string, std::function<bool(unsigned)>> predicates;
};