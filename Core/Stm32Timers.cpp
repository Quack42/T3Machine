#include "Stm32F407Platform.h"

#include "Stm32TimerData.h"

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim10; 	//generated in main.c

TimerData<Stm32F407Platform> timingManagerTimer(htim10);
