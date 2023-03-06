#pragma once

#include "TimingManager.h"
#include "Stm32F407Platform.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

template<>
class TimingManager<Stm32F407Platform> {
private:
	//Variables
	float pollWakeupTime = 1000.f; 	//Maximum consecutive period of sleep
	float timeUntilFirstWakeup = pollWakeupTime;
	volatile bool sleeping = false;
	//References
	TIM_HandleTypeDef & htim;

public:
	TimingManager(TIM_HandleTypeDef & htim) :
			htim(htim)
	{
	}

	void init();

	void setPollWakeupTime(float pollWakeupTime) {
		this->pollWakeupTime = pollWakeupTime;
	}

	void scheduleWakeup(float timeUntilWakeup) {
		if (timeUntilWakeup < timeUntilFirstWakeup) {
			timeUntilFirstWakeup = timeUntilWakeup;
		}
	}

	float sleep();

	void wakeup() {
		sleeping = false;
	}
};
