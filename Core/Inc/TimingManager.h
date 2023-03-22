#pragma once

#include "ProcessManager.h"
#include "TimerData.h"
#include "TimeValue.h"
#include "ConfigurableConstants.h"

#include <cstdint>


class TimedTask {
public:
	TimedTask * next = nullptr; 	//linked list stuff.. Don't really care about access here
private:
	ProcessRequest processRequest;
	TimeValue timeUntilTaskStart;
public:
	TimedTask(std::function<void(void)> func, TimeValue timeUntilTaskStart) :
			processRequest(func),
			timeUntilTaskStart(timeUntilTaskStart)
	{

	}

	void setTimeUntilTaskStart(TimeValue timeUntilTaskStart) {
		this->timeUntilTaskStart = timeUntilTaskStart;
	}

	void timeHasPassed(const TimeValue & timePassed) {
		timeUntilTaskStart -= timePassed;
	}

	bool isReadyToExecute() const {
		return (timeUntilTaskStart.us == 0) && (timeUntilTaskStart.ms == 0) && (timeUntilTaskStart.days == 0);
	}

	TimeValue getTimeUntilTaskStart() {
		return timeUntilTaskStart;
	}

	ProcessRequest & getProcessRequest() {
		return processRequest;
	}

};

template<typename Platform>
class TimingManager {
private:
	ProcessManager<Platform> & processManager;
	TimerData<Platform> & timerData;
	// float pollWakeupTime = 1000.f; 	//Maximum consecutive period of sleep
	// float timeUntilFirstWakeup = pollWakeupTime;
	volatile bool sleeping = false;

	// bool timerDone = false;
	uint32_t timerInterruptCount = 0;
	TimeValue timeSinceStart;

	TimedTask * firstTask = nullptr;
	TimedTask * firstTask_toAddList = nullptr;
	
public:
	TimingManager(ProcessManager<Platform> & processManager, TimerData<Platform> & timerData) :
			processManager(processManager),
			timerData(timerData),
			timeSinceStart(0,0,0)
	{

	}
	// void setPollWakeupTime(float pollWakeupTime);

	// void scheduleWakeup(float timeUntilWakeup);


	void addTask(TimedTask * task) {
		//Add task to to-add list
		processManager.disableInterrupts(); 	//Disable interrupts: This function can also be called from interrupts.
		task->next = firstTask_toAddList;
		firstTask_toAddList = task;
		processManager.enableInterrupts();
		//Awake the process so they are added without further delay.
		processManager.awake();
	}

	void setPollWakeupTime(float pollWakeupTime) {
		// this->pollWakeupTime = pollWakeupTime;
	}

	void scheduleWakeup(float timeUntilWakeup) {
		// if (timeUntilWakeup < timeUntilFirstWakeup) {
		// 	timeUntilFirstWakeup = timeUntilWakeup;
		// }
	}

	void waitTillNextTask();

	TimeValue getTimeSinceStart();

	void init();

	void timerISR();
	void start();
	// void _speedUp(); 	//TODO: REMOVE THIS

	//returns the time slept in ms
	// float sleep();

private:
	void updateTimeSinceStart();

	void addToAddTasksToTaskList() {
		processManager.disableInterrupts(); 	//Disable interrupts, because addTask can also be called by interrupts.
		TimedTask * taskToAdd = firstTask_toAddList;
		firstTask_toAddList = nullptr;
		processManager.enableInterrupts();

		while (taskToAdd != nullptr) {
			//store which task will be added next; so we're free to adjust taskToAdd's values
			TimedTask * nextTaskToAdd = taskToAdd->next;

			//Check if taskToAdd should be added in first position
			if ((firstTask == nullptr) || (firstTask->getTimeUntilTaskStart() > taskToAdd->getTimeUntilTaskStart())) {
				taskToAdd->next = firstTask;
				firstTask = taskToAdd;
			} else {
				//check the rest of the list
				TimedTask * current = firstTask;
				while ((current->next != nullptr) && (taskToAdd->getTimeUntilTaskStart() > current->next->getTimeUntilTaskStart())) {
					//while taskToAdd is later than current->next task, check the next one
					current = current->next;
				}
				//taskToAdd is BEFORE current->next task (or current->next is nullptr); so add it before, but after current
				taskToAdd->next = current->next;
				current->next = taskToAdd;
			}

			//check next
			taskToAdd = nextTaskToAdd;
		}
	}

	TimeValue _getTimeSinceStart();

	void updateTaskListWithTimePassage(TimeValue timePassed) {
		TimedTask * current = firstTask;
		while (current != nullptr) {
			current->timeHasPassed(timePassed);
			current = current->next;
		}
	}

};

#ifdef MOCK
	#include "MockTimingManager.h"
#endif //defined(MOCK)
//#ifdef STM
	// #include "Stm32TimingManager.h"
//#endif //defined(STM)
