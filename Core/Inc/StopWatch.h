#pragma once

#include "TimingManager.h"

template<typename Platform>
class StopWatch {
private:
	TimingManager<Platform> & timingManager;
public:
	StopWatch(TimingManager<Platform> & timingManager) :
			timingManager(timingManager)
	{
	}
};
