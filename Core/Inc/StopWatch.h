#pragma once

#include "TimingManager.h"

template<typename Platform>
class StopWatch {
private:
	TimingManager<Platform> & timingManager;
	TimeValue startTime;
public:
	StopWatch(TimingManager<Platform> & timingManager) :
			timingManager(timingManager),
			startTime(0,0,0)
	{
	}

	void start() {
		startTime = timingManager.getTimeSinceStart();
	}

	TimeValue getTime() {
		return timingManager.getTimeSinceStart() - startTime;
	}
};
