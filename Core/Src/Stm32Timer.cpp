#include "Timer.h"

#include "Stm32TimerData.h"
#include "Stm32F407Platform.h"
#include "ConfigurableConstants.h"

#include "stm32f4xx_hal.h"

#include <algorithm>
#include <cstdint>


// #include "MyGPIO.h" 	//TODO: REMOVE THIS
// #include "PlatformSelection.h" 	//TODO: REMOVE THIS
// extern OutputPin<Platform> ld6; 	//blue 	//TODO: REMOVE THIS
// extern OutputPin<Platform> ld5; 	//red 	//TODO: REMOVE THIS
// extern OutputPin<Platform> ld4; 	//grn 	//TODO: REMOVE THIS
// extern OutputPin<Platform> ld3; 	//ora 	//TODO: REMOVE THIS

template<>
void Timer<Stm32F407Platform>::init() {
	TIM_HandleTypeDef & timerHandle = timerData.getTimerHandle();
	timerHandle.Instance = timerConstants.getTimerBase();
	timerHandle.Init.Prescaler = timerConstants.getPrescaler()-1; 	//168MHz
	timerHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	timerHandle.Init.Period = 100000-1; 	//just some random number..
	timerHandle.Init.ClockDivision = timerConstants.getClockDivisionRegisterValue();
	timerHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	__HAL_TIM_URS_ENABLE(&timerHandle); 	//don't trigger interrupt by reinitializing CNT value
	HAL_TIM_Base_Init(&timerHandle);
}

template<>
void Timer<Stm32F407Platform>::setTime(TimeValue timeToWait) {

	//first determine whether this is more than the maximum time to wait

	const TimeValue kMaximumTimeToWait = timerConstants.getMaximumTimeToWait();

	TimeValue timeUntilNextInterrupt = timeToWait;
	if (timeUntilNextInterrupt > kMaximumTimeToWait) {
		//wait time is longer than maximum wait time.
		timeUntilNextInterrupt = kMaximumTimeToWait;
	}

	float ticksPerSecond = timerConstants.getTicksPerSecond();
	float ticksPerUS = ticksPerSecond / US_IN_A_S;
	float ticksPerMS = ticksPerSecond / MS_IN_A_S;
	float ticksPerDay = ticksPerSecond * S_IN_A_DAY;

	// uint32_t ticksToWait = 10*5000;
	uint32_t ticksToWait = std::max(2ul, static_cast<uint32_t>(0.5f + timeUntilNextInterrupt.us*ticksPerUS + timeUntilNextInterrupt.ms*ticksPerMS + timeUntilNextInterrupt.days*ticksPerDay));

	//TODO: disable interrupts
	//configure timer to use that period.
	TIM_HandleTypeDef & timerHandle = timerData.getTimerHandle();
	__HAL_TIM_SET_AUTORELOAD(&timerHandle, ticksToWait-1); 	 	//-1 because it counts 0 as a tick? Some example explained it, I understood it, forgot it, and now I just know it should be done.

	//assumes the timer was stopped; undefined behavior otherwise
	this->timeToWait = timeToWait;
	//TODO: enable interrupts
}

template<>
void Timer<Stm32F407Platform>::start() {
	TIM_HandleTypeDef & timerHandle = timerData.getTimerHandle();
	__HAL_TIM_CLEAR_FLAG(&timerHandle, TIM_FLAG_UPDATE); 	//clear interrupt flag
	HAL_TIM_Base_Start_IT(&timerHandle);
}

template<>
void Timer<Stm32F407Platform>::stop() {
	TIM_HandleTypeDef & timerHandle = timerData.getTimerHandle();
	HAL_TIM_Base_Stop_IT(&timerHandle);
}

template<>
void Timer<Stm32F407Platform>::_isr() {
	const TimeValue kMaximumTimeToWait = timerConstants.getMaximumTimeToWait();
	if (timeToWait > kMaximumTimeToWait) {
		timeToWait -= kMaximumTimeToWait;
		setTime(timeToWait);
	} else {
		timeToWait = TimeValue();
		// __HAL_TIM_DISABLE_IT(htim, TIM_IT_UPDATE);
		stop();
		isr();
	}
}
