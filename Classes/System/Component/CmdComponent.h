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
    
    void setTick(const std::string& tag,
             const std::function<State(unsigned eid, double dt)>& command,
             const std::function<void()>& onSuccess = nullptr,
             const std::function<void()>& onFailure = nullptr)
    {
        this->funcTickMap[tag] = { command, onSuccess, onFailure };
    }
    
    void askRemove(const std::string& tag)
    {
        this->ask4removeTag.push_back(tag);
    }
    
    void setAnimate(const std::string& tag,
                    const std::function<State(unsigned eid, double dt)>& command,
                    const std::function<void()>& onSuccess = nullptr,
                    const std::function<void()>& onFailure = nullptr)
    {
        this->funcAnimateMap[tag] = { command, onSuccess, onFailure };
    }
    
    void unsetAnimate(const std::string& tag)
    {
        this->funcAnimateMap.erase(tag);
    }
    
    void unsetTick(const std::string& tag)
    {
        this->funcTickMap.erase(tag);
    }
    
    void processAnimate(unsigned eid, double dt)
    {
        process(funcAnimateMap, eid, dt);
    }
    
    void processTick(unsigned eid, double dt)
    {
        process(funcTickMap, eid, dt);
    }
    
private:
    void process(std::map<std::string, CmdElement>& funcMap, unsigned eid, double dt)
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

        while (ask4removeTag.size() > 0)
        {
            auto tag = ask4removeTag.back();
            ask4removeTag.pop_back();
            unsetTick(tag);
            unsetAnimate(tag);
        }
    }

    std::map<std::string, CmdElement>   funcTickMap;
    std::map<std::string, CmdElement>   funcAnimateMap;
    std::list<std::string> ask4removeTag;
};