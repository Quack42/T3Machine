#include "Stm32TimingManager.h"
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
#define SECONDS_PER_TIMER_ITERATION_DURING_RUNNING_MODE (TIMER_TICKS_DURING_RUNNING / TICKS_PER_SECOND)
// #define SECONDS_PER_TIMER_ITERATION_DURING_RUNNING_MODE (static_cast<float>(TIMER_TICKS_DURING_RUNNING) / static_cast<float>(TICKS_PER_SECOND))

// bool timerDone = false;
// uint32_t timerInterruptCount = 0;
// uint32_t toCount = 0;
// uint64_t _isrCounts = 0;


//State Running: Process is running, Timer is keeping count of time spent
//State Waiting: Process is waiting for next time event or interrupt. Timer is counting up to timed-event moment.
//Function: GetTimeSinceStart: Add recorded time + timer counter time
//Actions: Timer interrupt occurs, add time to recorded time
//Action: other interrupt occurs -> another task needs the process and sleep is interrupted.
//State switch: Running->Waiting: Stop timer, add counter to time spent

//Get counter value
// uint32_t counterValue = __HAL_TIM_GET_AUTORELOAD(&timerData.getTimerHandle()); 	//counter value the timer just counted up to

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
	// timerHandle.Init.Period = TICKS_PER_SECOND-1; 	//1s
	// timerHandle.Init.Period = 50000-1;
	timerHandle.Init.Period = TIMER_TICKS_DURING_RUNNING-1;
	timerHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	timerHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	__HAL_TIM_URS_ENABLE(&timerHandle); 	//don't trigger interrupt by reinitializing CNT value
	HAL_TIM_Base_Init(&timerHandle);

	// postTimerInterruptProcessRequest = ProcessRequest(std::bind(&TimingManager<Stm32F407Platform>::postTimerInterrupt, this));
}


template<>
void TimingManager<Stm32F407Platform>::start() {
	HAL_TIM_Base_Init(&timerData.getTimerHandle());
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
	//TODO: timeToWait to timer ticks; if fitting.
	// timerHandle.Init.Period max value is 0xFFFF
	if (timeToWait > MaximumWaitTime) {
		// timerHandle.Init.Period = 50000;
		uint32_t ticksToWait = MaximumWaitTime.us/US_PER_TICK + MaximumWaitTime.ms*TICKS_PER_MS;
		timerHandle.Init.Period = ticksToWait;
	} else {
		uint32_t ticksToWait = timeToWait.us/US_PER_TICK + timeToWait.ms*TICKS_PER_MS;
		timerHandle.Init.Period = ticksToWait;
	}
	// timerHandle.Init.Period = 50000-1; 	//TODO: fill in task-time here
	HAL_TIM_Base_Init(&timerHandle); 	//NOTE: timerHandle reference is the exactly same as timerData.getTimerHandle(); this is just for readability
	//Ready 'timerDone' flag and sleep
	// timerDone = false;
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

	//Start timer to track how much time we spent in 'running' mode.
	timerHandle.Init.Period = TIMER_TICKS_DURING_RUNNING-1;
	HAL_TIM_Base_Init(&timerHandle); 	//NOTE: timerHandle reference is the exactly same as timerData.getTimerHandle(); this is just for readability
	HAL_TIM_Base_Start_IT(&timerData.getTimerHandle());


	//Check if any task is ready to start
	// if (timerDone) {
		//add task function to process.requestProcess list
	while (firstTask != nullptr && firstTask->isReadyToExecute()){
		processManager.requestProcess(firstTask->getProcessRequest());
		firstTask = firstTask->next;
	}
	// }
}



/////////////////////////////////////////////////////



// template<>
// float TimingManager<Stm32F407Platform>::sleep() {
// 	sleeping = true;


// 	//TODO: specify specific time to sleep
// 	// HAL_TIM_Base_Init(&timerData.getTimerHandle());

// 	while (sleeping) {
// 		//wait
// 	}

// 	//TODO: Continue here: make a timingManager that can serve the needs of a stopwatch and fire off processes at the requested times.


// 		// float countsOfSleep = __HAL_TIM_GET_COUNTER(&timerData.getTimerHandle());
// 		// HAL_TIM_Base_Stop_IT(&timerData.getTimerHandle());
// 		// timeSlept = (countsOfSleep) / 10.f;
// 	return 0.f;
// }

// template<>
// void TimingManager<Stm32F407Platform>::postTimerInterrupt() {
// 	processManager.disableInterrupts();
// 	uint64_t counterValue = _isrCounts;
// 	_isrCounts = 0;
// 	processManager.enableInterrupts();
// }

// template<>
// void TimingManager<Stm32F407Platform>::_speedUp() { 	//TODO: REMOVE THIS
// 	TIM_HandleTypeDef & timerHandle = timerData.getTimerHandle();
// 	timerHandle.Init.Period = 50000-1;

// 	HAL_TIM_Base_Init(&timerData.getTimerHandle());
// }

template<>
void TimingManager<Stm32F407Platform>::timerISR() {
	timerInterruptCount++;
	processManager.awake();
}

template<>
TimeValue TimingManager<Stm32F407Platform>::_getTimeSinceStart() {
	//get timer counter
	uint32_t counterValue = __HAL_TIM_GET_COUNTER(&timerData.getTimerHandle());
	//convert timer counter value to time units
	uint32_t usSinceCounterStart = counterValue*(US_IN_A_S / TICKS_PER_SECOND); 	//Note that this only counts what's in the timer CNT register

	//get timer iteration counter
	uint32_t secondsSinceCounterStart = timerInterruptCount * SECONDS_PER_TIMER_ITERATION_DURING_RUNNING_MODE; 	//Note that this counts how often the timer has looped its CNT value
	//Also note that SECONDS_PER_TIMER_ITERATION is a nice round number; there's only the ms to worry about here.

	//add time that timer has run to time-since-start-till-timer-start (i.e. count the time since start of the MCU till 'now')
	uint32_t usSinceTimeStart = timeSinceStart.us + usSinceCounterStart;
	uint32_t msSinceTimeStart = timeSinceStart.ms + usSinceTimeStart/US_IN_A_MS + secondsSinceCounterStart * MS_IN_A_S;
	usSinceTimeStart %= US_IN_A_MS;
	uint32_t daysSinceTimeStart = timeSinceStart.days + msSinceTimeStart/MS_IN_A_DAY;
	msSinceTimeStart %= MS_IN_A_DAY;

	//create return value
	return TimeValue(daysSinceTimeStart, msSinceTimeStart, usSinceTimeStart);
}

template<>
TimeValue TimingManager<Stm32F407Platform>::getTimeSinceStart() {
	processManager.disableInterrupts();
	TimeValue ret = _getTimeSinceStart();
	processManager.enableInterrupts();
	// //get timer counter
	// uint32_t counterValue = __HAL_TIM_GET_COUNTER(&timerData.getTimerHandle());
	// //convert timer counter value to time units
	// uint32_t usSinceCounterStart = counterValue*(US_IN_A_S / TICKS_PER_SECOND);

	// //TODO: using 'timeSinceStart' here isn't safe. It gets altered in an ISR
	// //add time that timer has run to time-since-start-till-timer-start (i.e. count the time since start of the MCU till 'now')
	// uint32_t usSinceTimeStart = timeSinceStart.us + usSinceCounterStart;
	// uint32_t msSinceTimeStart = timeSinceStart.ms + usSinceTimeStart/US_IN_A_MS;
	// usSinceTimeStart %= US_IN_A_MS;
	// uint32_t daysSinceTimeStart = timeSinceStart.days + msSinceTimeStart/MS_IN_A_DAY;
	// msSinceTimeStart %= MS_IN_A_DAY;

	// //create return value
	// TimeValue ret;
	// ret.us = usSinceTimeStart;
	// ret.us = msSinceTimeStart;
	// ret.us = daysSinceTimeStart;

	return ret;
}



//What: TimingManager that starts processes at specific times/intervals, and measures passage of time.


// void TimingManager<Stm32F407Platform>::init() {
// 	htim.Instance = TIM10;
// 	htim.Init.Prescaler = 16800-1;
// 	htim.Init.CounterMode = TIM_COUNTERMODE_UP;
// 	htim.Init.Period = 100000-1;
// 	htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
// 	htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
// }

// float TimingManager<Stm32F407Platform>::sleep() {
// 	const uint32_t sleepTime = timeUntilFirstWakeup*10.f; 	//in 0.1 ms
// 	float timeSlept = timeUntilFirstWakeup; 	//in ms
// 	timeUntilFirstWakeup = pollWakeupTime;
// 	if (sleepTime > 0) {

// //		htim.Init.Period = sleepTime+1;
// /*
// 		HAL_TIM_Base_Init(&htim);
// 		HAL_TIM_Base_Start_IT(&htim);
// 		while (sleeping) {
// 			//wait
// 		}
// 		HAL_TIM_Base_Stop_IT(&htim);*/
// 		HAL_TIM_Base_Init(&htim);
// 		sleeping = true;
// 		HAL_TIM_Base_Start_IT(&htim);
// 		while ((__HAL_TIM_GET_COUNTER(&htim) <= sleepTime) && sleeping) {
// 			//wait
// 		}
// 		float countsOfSleep = __HAL_TIM_GET_COUNTER(&htim);
// 		HAL_TIM_Base_Stop_IT(&htim);
// 		timeSlept = (countsOfSleep) / 10.f;
// 	} else {
// 		timeSlept = 0;
// 	}

// //	HAL_Delay(sleepTime); 	//TODO: use timer
// 	////HAL_SuspendTick();
// 	////HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
// 	return timeSlept;
// }

