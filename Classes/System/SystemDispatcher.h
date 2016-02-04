#pragma once

#include "Event.h"
#include "GateMap.h"
#include "WarpMap.h"
#include "CoreLib.h"
#include "Components.h"

struct SystemDispatcher
{
    //internal
    lib::Subject<void(unsigned)>                                  onSystemChanged;
    lib::Subject<void()>                                          onContextChanged;
    lib::Subject<void(unsigned, unsigned)>                        onEntityAdded;
    lib::Subject<void(unsigned, unsigned)>                        onEntityDeleted;
    lib::Subject<void(unsigned, unsigned)>                        onEntityPositionChanged;

    //control
    lib::Subject<void(unsigned, const lib::Nullable<cc::Vec2>&)>  onStickDirection;
    lib::Subject<void(unsigned, int)>                             onKeyPressDirection;
    lib::Subject<void(unsigned, int)>                             onKeyReleaseDirection;
    lib::Subject<void(ActionMode)>                                onSelectionAction;
    lib::Subject<void(cc::Point)>                                 onSelectionPos;
    
    //system
    lib::Subject<void(unsigned, unsigned, int)>                   onHealthChanged;
    lib::Subject<void(unsigned, unsigned, GateMap)>               onGateEnter;
    lib::Subject<void(unsigned, unsigned, GateMap)>               onGateTriggered;
    lib::Subject<void(unsigned, WarpMap)>                         onWarpTriggered;
    lib::Subject<void(unsigned, const cp::GearComponent&)>        onGearChanged;
};