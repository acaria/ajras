#pragma once

#include "Event.h"
#include "GateMap.h"
#include "WarpMap.h"
#include "CoreLib.h"
#include "SlotData.h"

struct SystemDispatcher
{
    //internal
    lib::Subject<void(unsigned)>                                  onSystemChanged;
    lib::Subject<void(unsigned)>                                  onSystemReady;
    lib::Subject<void()>                                          onContextChanged;
    lib::Subject<void(unsigned, unsigned)>                        onEntityAdded;
    lib::Subject<void(unsigned, unsigned)>                        onEntityDeleted;
    lib::Subject<void(unsigned, unsigned)>                        onEntityMoved;
    lib::Subject<void(unsigned, unsigned)>                        onEntityDeath;
    lib::Subject<void(unsigned, unsigned, const cc::Rect&)>       onFakeAgentRectAdded;
    lib::Subject<void(unsigned, unsigned)>                        onFakeAgentRectRemoved;
    lib::Subject<void(unsigned, cc::Node*, const cc::Rect&)>      onFakeAgentNodeAdded;
    lib::Subject<void(unsigned, cc::Node*)>                       onFakeAgentNodeRemoved;
    lib::Subject<void(unsigned, unsigned)>                        onEntityPositionChanged;

    //control
    lib::Subject<void(unsigned, const lib::Nullable<cc::Vec2>&)>  onStickDirection;
    lib::Subject<void(unsigned, int)>                             onKeyPressDirection;
    lib::Subject<void(unsigned, int)>                             onKeyReleaseDirection;
    lib::Subject<void(cc::Point)>                                 onSelectionPos;
    
    //system
    lib::Subject<void(unsigned, unsigned, float)>                 onHealthChanged;
    lib::Subject<void(unsigned, unsigned, float)>                 onStaminaChanged;
    lib::Subject<void(unsigned, unsigned, GateMap)>               onGateEnter;
    lib::Subject<void(unsigned, unsigned, GateMap)>               onGateEnterAfter;
    lib::Subject<void(unsigned, unsigned, GateMap)>               onGateTriggered;
    lib::Subject<void(unsigned, WarpMap)>                         onWarpTriggered;
    lib::Subject<void(unsigned, SlotData*)>                       onGearSlotChanged;
    lib::Subject<void(unsigned, unsigned)>                        onGearCurrencyChanged;
};