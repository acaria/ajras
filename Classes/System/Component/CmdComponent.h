#pragma once

struct CmdComponent
{
    enum class State
    {
        success,
        failure,
        inProgress
    };
    
    void set(const std::string& tag, const std::function<State(unsigned eid, double dt)>& command)
    {
        this->funcMap[tag] = command;
        this->stateMap[tag] = State::inProgress;
    }
    
    State get(const std::string& tag)
    {
        if (!lib::hasKey(stateMap, tag))
            return State::failure;
        return stateMap[tag];
    }
    
    unsigned countInProgress()
    {
        unsigned count = 0;
        for(auto pair : stateMap)
        {
            if (pair.second == State::inProgress)
                count++;
        }
        return count;
    }
    
    void process(unsigned eid, double dt)
    {
        for(auto& pair : stateMap)
        {
            if (pair.second == State::inProgress)
                pair.second = funcMap[pair.first](eid, dt);
        }
    }
    
    void reset()
    {
        funcMap.clear();
        stateMap.clear();
    }
    
    std::map<std::string, std::function<State(unsigned, double)>>  funcMap;
    std::map<std::string, State>                                   stateMap;
};