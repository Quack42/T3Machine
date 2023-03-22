#pragma once

#include "ProcessManager.h"
#include "TimerData.h"
#include "TimeValue.h"
#include "ConfigurableConstants.h"


#include "TimedTask.h"


template<typename Platform>
class TimingManager {
private:
	//references
	ProcessManager<Platform> & processManager;
	TimerData<Platform> & timerData;

	uint32_t timerInterruptCount = 0;
	TimeValue timerCycleTime;
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

	void addTask(TimedTask * task) {
		if (toAddListContainsTask(task)) {
			//already in to-add list
			return;
		}
		//Add task to to-add list
		processManager.disableInterrupts(); 	//Disable interrupts: This function can also be called from interrupts.
		task->next = firstTask_toAddList;
		firstTask_toAddList = task;
		processManager.enableInterrupts();
		//Awake the process so they are added without further delay.
		processManager.awake();
	}

	void waitTillNextTask();

	TimeValue getTimeSinceStart();

	void init();

	void timerISR();
	void start();

private:
	bool toAddListContainsTask(TimedTask * task) {
		TimedTask * currentToAdd = firstTask_toAddList;
		while ((currentToAdd != nullptr) && (currentToAdd != task)) {
			currentToAdd = currentToAdd->next;
		}
		return (currentToAdd == task);
	}

	bool taskListContainsTask(TimedTask * task) {
		TimedTask * current = firstTask;
		while ((current != nullptr) && (current != task)) {
			current = current->next;
		}
		return (current == task);
	}

	void updateTimeSinceStart();

	void addToAddTasksToTaskList() {
		processManager.disableInterrupts(); 	//Disable interrupts, because addTask can also be called by interrupts.
		TimedTask * taskToAdd = firstTask_toAddList;
		firstTask_toAddList = nullptr;
		processManager.enableInterrupts();

		while (taskToAdd != nullptr) {
			//store which task will be added next; so we're free to adjust taskToAdd's values
			TimedTask * nextTaskToAdd = taskToAdd->next;
			if (!taskListContainsTask(taskToAdd)) {
				//Task isn't already added

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
