#pragma once
#include "ECSGroup.h"
#include "SysHelper.h"
#include "SystemContext.h"
#include "SystemDispatcher.h"

class SystemFacade;

class BaseSystem
{
    friend SystemFacade;
public:
    virtual ~BaseSystem() {}
    
    virtual void tick(double dt) = 0;
    virtual void animate(double dt, double tp) = 0;
    
protected:
    void prepare(SystemContext* context, SystemDispatcher* dispatcher)
    {
        this->context = context;
        this->dispatcher = dispatcher;
        this->init();
    }
    
    virtual void init() {};

    SystemContext*                  context;
    SystemDispatcher*               dispatcher;
    std::list<lib::Registration>    eventRegs;
};