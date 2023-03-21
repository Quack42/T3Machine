// #pragma once

// #include "TimingManager.h"
// #include "Stm32F407Platform.h"

// #include "stm32f4xx_hal.h"
// #include "stm32f4xx_hal_tim.h"

// template<>
// class TimingManager<Stm32F407Platform> {
// 	//TODO: Remove the Stm32TimingManager.h declaration; keep TimingManager.h declaration as one-fits-all and use Stm32TimingManager.cpp to implement platform dependent logic. Introduce PlatformTimingManagerData<Platform> data block (or something a little shorter) in header.
// private:
// 	//Variables
// 	float pollWakeupTime = 1000.f; 	//Maximum consecutive period of sleep
// 	float timeUntilFirstWakeup = pollWakeupTime;
// 	volatile bool sleeping = false;
// 	//References
// 	TIM_HandleTypeDef & htim;

// public:
// 	TimingManager(TIM_HandleTypeDef & htim) :
// 			htim(htim)
// 	{
// 	}

// 	void init();

// 	void setPollWakeupTime(float pollWakeupTime) {
// 		this->pollWakeupTime = pollWakeupTime;
// 	}

// 	void scheduleWakeup(float timeUntilWakeup) {
// 		if (timeUntilWakeup < timeUntilFirstWakeup) {
// 			timeUntilFirstWakeup = timeUntilWakeup;
// 		}
// 	}

// 	float sleep();
// 	void start();
// 	void timerISR();
// 	void _speedUp(); 	//TODO: remove this

// 	void wakeup() {
// 		sleeping = false;
// 	}
// };
