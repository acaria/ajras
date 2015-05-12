#pragma once

class TickScheduler : public cc::Node
{
public:
    TickScheduler() {}
    virtual ~TickScheduler() {}
    
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExitTransitionDidStart() override;
    void update(float delta) override;
};