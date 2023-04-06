#pragma once

#include "PlatformSelection.h"
#include "TimerData.h"
#include "Timer.h"

extern TimerData<Platform> timingManagerTimerData;
extern const TimerConstants<Platform> timingManagerTimerConstants;
extern TimerData<Platform> steppingTaskTimerData;
extern const TimerConstants<Platform> steppingTaskTimerConstants;


extern Timer<Platform> steppingTaskTimer;

