#include "CmdSystem.h"

void CmdSystem::init()
{
    this->eventRegs.push_back(this->dispatcher->onEntityMoved.registerObserver([](unsigned group, unsigned eid){
        if (ecs::has<cp::Cmd>(eid))
        {
            auto& cpCmd = ecs::get<cp::Cmd>(eid);
            cpCmd.askRemove("goto");
            cpCmd.askRemove("goby");
        }
    }));
}

void CmdSystem::animate(double dt, double tp)
{
    for(auto eid : context->ecs->system<cp::Cmd>())
    {
        ecs::get<cp::Cmd>(eid).processAnimate(eid, dt);
    }
}

void CmdSystem::tick(double dt)
{
    auto cmds = context->ecs->system<cp::Cmd>();
    for(auto eid : cmds)
    {
        ecs::get<cp::Cmd>(eid).processTick(eid, dt);
    }
}
