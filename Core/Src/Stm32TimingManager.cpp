#include "Stm32TimingManager.h"
#include "TimingManager.h"
#include "Stm32F407Platform.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

//Check RCC_APB1ENR, and RCC_APB2ENR register in Reference Manual (RM0090) to find out which TIM's go with which APB's (for input clock frequency).

//TIM_HandleTypeDef tim2_initStruct = {0};
void TimingManager<Stm32F407Platform>::init() {
	htim.Instance = TIM10;
	htim.Init.Prescaler = 16800-1;
	htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim.Init.Period = 100000-1;
	htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
}

float TimingManager<Stm32F407Platform>::sleep() {
	const uint32_t sleepTime = timeUntilFirstWakeup*10.f; 	//in 0.1 ms
	float timeSlept = timeUntilFirstWakeup; 	//in ms
	timeUntilFirstWakeup = pollWakeupTime;
	if (sleepTime > 0) {

//		htim.Init.Period = sleepTime+1;
/*
		HAL_TIM_Base_Init(&htim);
		HAL_TIM_Base_Start_IT(&htim);
		while (sleeping) {
			//wait
		}
		HAL_TIM_Base_Stop_IT(&htim);*/
		HAL_TIM_Base_Init(&htim);
		sleeping = true;
		HAL_TIM_Base_Start_IT(&htim);
		while ((__HAL_TIM_GET_COUNTER(&htim) <= sleepTime) && sleeping) {
			//wait
		}
		float countsOfSleep = __HAL_TIM_GET_COUNTER(&htim);
		HAL_TIM_Base_Stop_IT(&htim);
		timeSlept = (countsOfSleep) / 10.f;
	} else {
		timeSlept = 0;
	}

//	HAL_Delay(sleepTime); 	//TODO: use timer
	////HAL_SuspendTick();
	////HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	return timeSlept;
}

