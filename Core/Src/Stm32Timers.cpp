#include "Stm32F407Platform.h"

#include "Stm32TimerData.h"

#include "stm32f4xx_hal.h"

#include "ConfigurableConstants.h"

#include "Timer.h"

// Platform specific handles
extern TIM_HandleTypeDef htim10; 	//generated in main.c
extern TIM_HandleTypeDef htim11; 	//generated in main.c

// Platform specific information
// constexpr TimerData<Stm32F407Platform> timingManagerTimer(htim10, TIM10_BASE, kSystemFrequency, (kSystemFrequency/kTimingManagerTicksPerSecond), TIM_CLOCKDIVISION_DIV1);
TimerData<Stm32F407Platform> timingManagerTimerData(htim11);
extern constexpr TimerConstants<Stm32F407Platform> timingManagerTimerConstants(TIM11_BASE, kSystemFrequency, (kSystemFrequency/kTimingManagerTicksPerSecond), TIM_CLOCKDIVISION_DIV1);

TimerData<Stm32F407Platform> steppingTaskTimerData(htim10);
extern constexpr TimerConstants<Stm32F407Platform> steppingTaskTimerConstants(TIM10_BASE, kSystemFrequency, (kSystemFrequency/kSteppingTaskTicksPerSecond), TIM_CLOCKDIVISION_DIV1);

// The timers.
Timer<Stm32F407Platform> steppingTaskTimer(steppingTaskTimerData, steppingTaskTimerConstants);

