#pragma once

#include "PlatformSelection.h"
#include "TimerData.h"
#include "Timer.h"

extern TimerData<Platform> timingManagerTimerData;
extern const TimerConstants<Platform> timingManagerTimerConstants;

extern TimerData<Platform> steppingTaskXTimerData;
extern const TimerConstants<Platform> steppingTaskXTimerConstants;

extern TimerData<Platform> steppingTaskYTimerData;
extern const TimerConstants<Platform> steppingTaskYTimerConstants;

extern TimerData<Platform> steppingTaskZTimerData;
extern const TimerConstants<Platform> steppingTaskZTimerConstants;


extern Timer<Platform> steppingTaskXTimer;
extern Timer<Platform> steppingTaskYTimer;
extern Timer<Platform> steppingTaskZTimer;

