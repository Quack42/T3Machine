#pragma once

#include "TimerData.h"
#include "stm32f4xx_hal.h"
#include "Stm32F407Platform.h"

template<>
class TimerData<Stm32F407Platform> {
private:
	TIM_HandleTypeDef & timerHandle;
public:
	TimerData(TIM_HandleTypeDef & timerHandle) :
			timerHandle(timerHandle)
	{

	}

	TIM_HandleTypeDef & getTimerHandle() const {
		return timerHandle;
	}
};
