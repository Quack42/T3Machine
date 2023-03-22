#pragma once

#include "ProcessRequest.h"
#include "TimeValue.h"

#include <functional>

class TimedTask {
public:
	TimedTask * next = nullptr; 	//public member variable.. //TODO: fixme
private:
	ProcessRequest processRequest;
	TimeValue timeUntilTaskStart;
public:
	TimedTask(std::function<void(void)> func, TimeValue timeUntilTaskStart) :
			processRequest(func),
			timeUntilTaskStart(timeUntilTaskStart)
	{

	}

	void setTimeUntilTaskStart(TimeValue timeUntilTaskStart) {
		this->timeUntilTaskStart = timeUntilTaskStart;
	}

	void timeHasPassed(const TimeValue & timePassed) {
		timeUntilTaskStart -= timePassed;
	}

	bool isReadyToExecute() const {
		return (timeUntilTaskStart.us == 0) && (timeUntilTaskStart.ms == 0) && (timeUntilTaskStart.days == 0);
	}

	TimeValue getTimeUntilTaskStart() {
		return timeUntilTaskStart;
	}

	ProcessRequest & getProcessRequest() {
		return processRequest;
	}

};
