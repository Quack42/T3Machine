//What: TimingManager that starts processes at specific times/intervals, and measures passage of time.

//State Running: Process is running, Timer is keeping count of time spent
//State Waiting: Process is waiting for next timed event or interrupt. Timer is counting up to timed-event moment (and keeping track of time spent that way).

#include "TimingManager.h"
#include "Stm32F407Platform.h"

#include "Stm32TimerData.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

//Check RCC_APB1ENR, and RCC_APB2ENR register in Reference Manual (RM0090) to find out which TIM's go with which APB's (for input clock frequency).

#define SYSTEM_HZ (168000000)
#define TICKS_PER_SECOND (10000)
#define TICKS_PER_MS (10)
#define US_PER_TICK (100)
#define TIMER_PRESCALER ((SYSTEM_HZ) / (TICKS_PER_SECOND))

#define TIMER_TICKS_DURING_RUNNING (100000)
#define SECONDS_PER_TIMER_ITERATION_DURING_RUNNING_MODE (TIMER_TICKS_DURING_RUNNING / TICKS_PER_SECOND) 	//make sure this division doesn't end up in a fraction

uint32_t ticksToUS(uint32_t ticks){
	uint32_t us = ticks*(US_IN_A_S / TICKS_PER_SECOND);
	return us;
}

TimeValue ticksToTimeValue(uint32_t ticks) {
	uint32_t us = ticksToUS(ticks);
	return TimeValue(0, us/US_IN_A_MS, us%US_IN_A_MS);
}

//declare specialization BEFORE use
template<>
TimeValue TimingManager<Stm32F407Platform>::_getTimeSinceStart();
template<>
TimeValue TimingManager<Stm32F407Platform>::getTimeSinceStart();

//TIM_HandleTypeDef tim2_initStruct = {0};
template<>
void TimingManager<Stm32F407Platform>::init() {
	TIM_HandleTypeDef & timerHandle = timerData.getTimerHandle();
	timerHandle.Instance = TIM10;
	timerHandle.Init.Prescaler = TIMER_PRESCALER-1; 	//168MHz
	timerHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	timerHandle.Init.Period = TIMER_TICKS_DURING_RUNNING-1;
	timerHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	timerHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	__HAL_TIM_URS_ENABLE(&timerHandle); 	//don't trigger interrupt by reinitializing CNT value
	timerCycleTime = ticksToTimeValue(TIMER_TICKS_DURING_RUNNING);
	HAL_TIM_Base_Init(&timerHandle);
}


template<>
void TimingManager<Stm32F407Platform>::start() {
	TIM_HandleTypeDef & timerHandle = timerData.getTimerHandle();
	timerHandle.Init.Period = TIMER_TICKS_DURING_RUNNING-1;
	HAL_TIM_Base_Init(&timerData.getTimerHandle());
	timerCycleTime = ticksToTimeValue(TIMER_TICKS_DURING_RUNNING);
	HAL_TIM_Base_Start_IT(&timerData.getTimerHandle());
}


//NOTE: DON'T call from ISR
template<>
void TimingManager<Stm32F407Platform>::updateTimeSinceStart() {
	//disable interrupts so nothing uses "getTimeSinceStart" at an invalid moment; needs to be before the next line
	processManager.disableInterrupts();
	//Update timeSinceStart
	timeSinceStart = _getTimeSinceStart(); 	//simply use the calculated time to record the new one. From this point on the response of getTimeSinceStart() is invalid.
	//Reset timer-based tracking of time
	timerInterruptCount = 0;
	__HAL_TIM_SET_COUNTER(&timerData.getTimerHandle(), 0);
	//From this point on, the response of getTimeSinceStart() is valid again, so we can re-enable interrupts.
	processManager.enableInterrupts();
}

template<>
void TimingManager<Stm32F407Platform>::waitTillNextTask() {
	static constexpr TimeValue MaximumWaitTime(0, 5000, 0); 		//Choose this as maximum timer time as it seemed nicely convenient round number.
	// constexpr TimeValue MaximumWaitTime(0, 6553, 5); 	//this is the actual maximum time according to the comments in the HAL, although reality seems different.
	
	///Update tasks
	//Stop timer
	HAL_TIM_Base_Stop_IT(&timerData.getTimerHandle());
	//Add time passed to tracked time for all tasks
	TimeValue timeDifference;
	timeDifference = getTimeSinceStart() - timeSinceStart;
	updateTaskListWithTimePassage(timeDifference);
	///Add to recorded time and reset;
	updateTimeSinceStart();
	//Add to-add tasks to list
	addToAddTasksToTaskList();
	//Remove to-remove tasks from list
	// _removeToRemoveTasksFromTaskList(); 	//Run this AFTER addToAddTasksToTaskList

	///Get earliest task wait-time
	TimeValue timeToWait;
	if (firstTask != nullptr) {
		timeToWait = firstTask->getTimeUntilTaskStart();
	} else {
		timeToWait = DefaultTimeToWait;
	}

	///Start sleep-time for earliest task
	//Set sleep time
	TIM_HandleTypeDef & timerHandle = timerData.getTimerHandle();
	// timerHandle.Init.Period max value is 0xFFFF
	if (timeToWait > MaximumWaitTime) {
		uint32_t ticksToWait = MaximumWaitTime.us/US_PER_TICK + MaximumWaitTime.ms*TICKS_PER_MS;
		timerHandle.Init.Period = ticksToWait-1;
	} else {
		uint32_t ticksToWait = timeToWait.us/US_PER_TICK + timeToWait.ms*TICKS_PER_MS;
		timerHandle.Init.Period = ticksToWait-1; 	//-1 because it counts 0 as a tick? Some example explained it, I understood it, forgot it, and now I just know it should be done.
	}
	//Start timer
	HAL_TIM_Base_Init(&timerHandle); 	//NOTE: timerHandle reference is the exactly same as timerData.getTimerHandle(); this is just for readability
	timerCycleTime = timeToWait;
	HAL_TIM_Base_Start_IT(&timerData.getTimerHandle());
	//Sleep
	processManager.sleep();
	// Two options can occur after this:
	// - timerISR calls processManager.awake(); The timer is done and we can update the timeSinceStart again.
	// - an interrupt other than the TimeManager timer calls processManager.awake().

	//Stop timer
	HAL_TIM_Base_Stop_IT(&timerData.getTimerHandle());
	//Add time passed to time for all tasks (except the 'to-add' list)
	timeDifference = getTimeSinceStart() - timeSinceStart;
	updateTaskListWithTimePassage(timeDifference);
	//Update the timeSinceStart value again
	updateTimeSinceStart();
	//Add to-add tasks to list
	addToAddTasksToTaskList();
	//Remove to-remove tasks from list
	// _removeToRemoveTasksFromTaskList(); 	//Run this AFTER addToAddTasksToTaskList

	//Start timer to track how much time we spent in 'running' mode.
	timerHandle.Init.Period = TIMER_TICKS_DURING_RUNNING-1;
	HAL_TIM_Base_Init(&timerHandle); 	//NOTE: timerHandle reference is the exactly same as timerData.getTimerHandle(); this is just for readability
	timerCycleTime = ticksToTimeValue(TIMER_TICKS_DURING_RUNNING);
	HAL_TIM_Base_Start_IT(&timerData.getTimerHandle());

	//Check if any task is ready to start
	while (firstTask != nullptr && firstTask->isReadyToExecute()){
		processManager.requestProcess(firstTask->getProcessRequest());
		firstTask = firstTask->next;
	}
}



template<>
void TimingManager<Stm32F407Platform>::timerISR() {
	timerInterruptCount++;
	processManager.awake();
}

template<>
TimeValue TimingManager<Stm32F407Platform>::_getTimeSinceStart() {
	//Assumes timer interrupt is disabled

	//get timer counter
	uint32_t counterValue = __HAL_TIM_GET_COUNTER(&timerData.getTimerHandle());
	//convert timer counter value to time units
	uint32_t usSinceCounterStart = ticksToUS(counterValue); 	//Note that this only counts what's in the timer CNT register

	//get timer iteration counter
	TimeValue timerCyclesInTime = timerInterruptCount * timerCycleTime; 	//Note that this counts how often the timer has looped its CNT value 	//TODO: This is wrong; while waiting for a timed-event it will hardly ever take SECONDS_PER_TIMER_ITERATION_DURING_RUNNING_MODE seconds to loop the timer
	// uint32_t secondsSinceCounterStart = timerInterruptCount * SECONDS_PER_TIMER_ITERATION_DURING_RUNNING_MODE; 	//Note that this counts how often the timer has looped its CNT value 	//TODO: This is wrong; while waiting for a timed-event it will hardly ever take SECONDS_PER_TIMER_ITERATION_DURING_RUNNING_MODE seconds to loop the timer
	//Also note that SECONDS_PER_TIMER_ITERATION is a nice round number; there's only the ms to worry about here.

	//add time that timer has run to time-since-start-till-timer-start (i.e. count the time since start of the MCU till 'now')
	uint32_t usSinceTimeStart = timeSinceStart.us + usSinceCounterStart;
	uint32_t msSinceTimeStart = timeSinceStart.ms + usSinceTimeStart/US_IN_A_MS;
	// uint32_t msSinceTimeStart = timeSinceStart.ms + usSinceTimeStart/US_IN_A_MS + secondsSinceCounterStart * MS_IN_A_S;
	usSinceTimeStart %= US_IN_A_MS;
	uint32_t daysSinceTimeStart = timeSinceStart.days + msSinceTimeStart/MS_IN_A_DAY;
	msSinceTimeStart %= MS_IN_A_DAY;

	//create return value
	return TimeValue(daysSinceTimeStart, msSinceTimeStart, usSinceTimeStart) + timerCyclesInTime;
}

template<>
TimeValue TimingManager<Stm32F407Platform>::getTimeSinceStart() {
	processManager.disableInterrupts();
	TimeValue ret = _getTimeSinceStart();
	processManager.enableInterrupts();
	return ret;
}
