#pragma once

template<typename Platform>
class TimingManager {
public:
	void setPollWakeupTime(float pollWakeupTime) {}

	void scheduleWakeup(float timeUntilWakeup) {}

	void init() { ;;; }

	//returns the time slept in ms
	float sleep() {return 0;}
};

#ifdef MOCK
	#include "MockTimingManager.h"
#endif //defined(MOCK)
//#ifdef STM
	#include "Stm32TimingManager.h"
//#endif //defined(STM)
