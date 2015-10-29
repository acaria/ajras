#pragma once

class DebugInfoLayer : public cc::Node
{
public:
    bool init() override;
    
    void update();
    
private:
    cc::Label* info;
};