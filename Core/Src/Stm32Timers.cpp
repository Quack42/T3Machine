#include "Stm32F407Platform.h"

#include "Stm32TimerData.h"

#include "stm32f4xx_hal.h"

#include "ConfigurableConstants.h"

#include "Timer.h"

//Check RCC_APB1ENR, and RCC_APB2ENR register in Reference Manual (RM0090) to find out which TIM's go with which APB's (for input clock frequency).


// Platform specific handles
extern TIM_HandleTypeDef htim9; 	//generated in main.c
extern TIM_HandleTypeDef htim10; 	//generated in main.c
extern TIM_HandleTypeDef htim11; 	//generated in main.c
extern TIM_HandleTypeDef htim12; 	//generated in main.c

// Platform specific information
TimerData<Stm32F407Platform> timingManagerTimerData(htim10);
extern constexpr TimerConstants<Stm32F407Platform> timingManagerTimerConstants(TIM10_BASE, kAPB2TimerFrequency, (kAPB2TimerFrequency/kTimingManagerTicksPerSecond), TIM_CLOCKDIVISION_DIV1);

TimerData<Stm32F407Platform> steppingTaskXTimerData(htim9);
extern constexpr TimerConstants<Stm32F407Platform> steppingTaskXTimerConstants(TIM9_BASE, kAPB2TimerFrequency, (kAPB2TimerFrequency/kSteppingTaskTicksPerSecond), TIM_CLOCKDIVISION_DIV1);

TimerData<Stm32F407Platform> steppingTaskYTimerData(htim11);
extern constexpr TimerConstants<Stm32F407Platform> steppingTaskYTimerConstants(TIM11_BASE, kAPB2TimerFrequency, (kAPB2TimerFrequency/kSteppingTaskTicksPerSecond), TIM_CLOCKDIVISION_DIV1);

TimerData<Stm32F407Platform> steppingTaskZTimerData(htim12);
extern constexpr TimerConstants<Stm32F407Platform> steppingTaskZTimerConstants(TIM12_BASE, kAPB1TimerFrequency, (kAPB1TimerFrequency/kSteppingTaskTicksPerSecond), TIM_CLOCKDIVISION_DIV1);


// TimerData<Stm32F407Platform> steppingTaskXTimerData(htim9);
// extern constexpr TimerConstants<Stm32F407Platform> steppingTaskXTimerConstants(TIM9_BASE, kAPB2TimerFrequency, (kAPB2TimerFrequency/kSteppingTaskTicksPerSecond), TIM_CLOCKDIVISION_DIV1);

// TimerData<Stm32F407Platform> steppingTaskYTimerData(htim11);
// extern constexpr TimerConstants<Stm32F407Platform> steppingTaskYTimerConstants(TIM11_BASE, kAPB2TimerFrequency, (kAPB2TimerFrequency/kSteppingTaskTicksPerSecond), TIM_CLOCKDIVISION_DIV1);

// TimerData<Stm32F407Platform> steppingTaskZTimerData(htim12);
// extern constexpr TimerConstants<Stm32F407Platform> steppingTaskZTimerConstants(TIM12_BASE, kAPB1TimerFrequency, (kAPB1TimerFrequency/kSteppingTaskTicksPerSecond), TIM_CLOCKDIVISION_DIV1);

// The timers.
Timer<Stm32F407Platform> steppingTaskXTimer(steppingTaskXTimerData, steppingTaskXTimerConstants);
Timer<Stm32F407Platform> steppingTaskYTimer(steppingTaskYTimerData, steppingTaskYTimerConstants);
Timer<Stm32F407Platform> steppingTaskZTimer(steppingTaskZTimerData, steppingTaskZTimerConstants);

