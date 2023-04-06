#pragma once

#include "TimerData.h"
#include "TimeValue.h"

#include <functional>

template<typename Platform>
class Timer {
private:
	TimerData<Platform> & timerData;
	const TimerConstants<Platform> & timerConstants;
	TimeValue timeToWait;

	std::function<void(void)> isrCallbackFunction;
public:
	Timer(TimerData<Platform> & timerData, const TimerConstants<Platform> & timerConstants) :
			timerData(timerData),
			timerConstants(timerConstants)
	{
	}

	void init();
	void setTime(TimeValue timeValue);
	void start();
	void stop();

	void _isr(); 	//call from platform specific ISR.

	void setISRCallbackFunction(std::function<void(void)> isrCallbackFunction) {
		this->isrCallbackFunction = isrCallbackFunction;
	}



private:
	void isr() { 	//NOTE: call this only from _isr()
		if (isrCallbackFunction) {
			isrCallbackFunction();
		}
	}
};
