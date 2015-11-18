#pragma once

struct CommandComponent
{
    std::function<bool(unsigned)> process = nullptr;
    
    void set(const std::function<bool(unsigned eid)>& command)
    {
        this->process = command;
    }
};