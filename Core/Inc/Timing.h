#pragma once

#include "TimingManager.h"

class TimeCountDown {
private:
	float timeCountDown;
public:
	TimeCountDown() :
			timeCountDown(0)
	{
	}

	TimeCountDown(float initialValue_timeCountDown) :
			timeCountDown(initialValue_timeCountDown)
	{
	}

	void countDown(float time) {
		timeCountDown -= time;
	}

	bool isReady() const {
		return timeCountDown <= 0;
	}

	void reset() {
		timeCountDown = 0;
	}

	void addTimeCountDown(float timeCountDown) {
		this->timeCountDown += timeCountDown;
	}

	void setTimeCountDown(float timeCountDown) {
		this->timeCountDown = timeCountDown;
	}

	float getRemainingTime() const {
		return timeCountDown;
	}
};
