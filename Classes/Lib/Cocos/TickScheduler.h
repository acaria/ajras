#pragma once

class TickScheduler : public cc::Node
{
public:
    TickScheduler(std::function<void(float)> updateFunc);
    virtual ~TickScheduler() {}
    
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExitTransitionDidStart() override;
    void update(float delta) override;
    
private:
    std::function<void(float)> onUpdate;
};