#pragma once

struct CmdComponent
{
    enum class State
    {
        success,
        failure,
        inProgress
    };
    
    struct CmdElement
    {
        std::function<State(unsigned eid, double dt)> onProcess;
        std::function<void()> onSuccess;
        std::function<void()> onFailure;
    };
    
    void set(const std::string& tag,
             const std::function<State(unsigned eid, double dt)>& command,
             const std::function<void()>& onSuccess = nullptr,
             const std::function<void()>& onFailure = nullptr)
    {
        this->funcMap[tag] = { command, onSuccess, onFailure };
    }
    
    void process(unsigned eid, double dt)
    {
        for(auto it = funcMap.begin(); it != funcMap.end(); /*no increment*/)
        {
            auto& el = it->second;
            switch(el.onProcess(eid, dt))
            {
                case State::success:
                    if (el.onSuccess != nullptr) el.onSuccess();
                    it = funcMap.erase(it);
                    break;
                case State::failure:
                    if (el.onFailure != nullptr) el.onFailure();
                    it = funcMap.erase(it);
                    break;
                case State::inProgress:
                    ++it;
                    break;
            }
        }
    }
    
    std::map<std::string, CmdElement>   funcMap;
};