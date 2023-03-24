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
	// TimedTask * firstTask_toRemoveList = nullptr;
	
public:
	TimingManager(ProcessManager<Platform> & processManager, TimerData<Platform> & timerData) :
			processManager(processManager),
			timerData(timerData),
			timeSinceStart(0,0,0)
	{

	}

	void addTask(TimedTask * task) {
		//prevent looped-linked-list
		if (toAddListContainsTask(task) || taskListContainsTask(task)) {
			//already in to-add list or actual running list
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

	// void removeTask(TimedTask * taskToRemove) {
	// 	if (taskToRemove == nullptr) {
	// 		return;
	// 	}
	// 	//Add task to to-remove list
	// 	processManager.disableInterrupts(); 	//Disable interrupts: This function can also be called from interrupts.
	// 	taskToRemove->next = firstTask_toRemoveList;
	// 	firstTask_toRemoveList = taskToRemove;
	// 	processManager.enableInterrupts();
	// }

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

	// bool toRemoveListContainsTask(TimedTask * task) {
	// 	TimedTask * currentToRemove = firstTask_toRemoveList;
	// 	while ((currentToRemove != nullptr) && (currentToRemove != task)) {
	// 		currentToRemove = currentToRemove->next;
	// 	}
	// 	return (currentToRemove == task);
	// }

	void updateTimeSinceStart();

	void addToAddTasksToTaskList() {
		//Unfortunately we can't just uncouple to-add list from firstTask_toAddList, so we can work on them without having to worry about interrupts.
		// Adding an element to the to-add list must also check whether the element isn't already in the tasklist, about to be executed.
		// Therefore, we can't allow any interrupts (which might potentially call addTask) during the process of adding the to-add list to the task list
		// Luckily, the toAdd list usually isn't that long.
		processManager.disableInterrupts(); 	//Disable interrupts, because addTask can also be called by interrupts.
		TimedTask * taskToAdd = firstTask_toAddList;
		firstTask_toAddList = nullptr;
		processManager.enableInterrupts();

		while (taskToAdd != nullptr) {
			//store which task will be added next; so we're free to adjust taskToAdd's values
			TimedTask * nextTaskToAdd = taskToAdd->next;
			if (!taskListContainsTask(taskToAdd))  	//I'm not sure this is necessary. Because the element was already present in another linked list, it cannot be present here (or we'd have a corrupt linked list somehow)
			{
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
	// void _removeToRemoveTasksFromTaskList() {
	// 	//NOTE: Run this AFTER addToAddTasksToTaskList 	--> Because of this, we won't have to check to-add list.
	// 	//NOTE: only call this from 'waitTillNextTask()'; and never from an interrupt
	// 	//uncouple to-remove list from firstTask_toRemoveList, so we can work on them without having to worry about interrupts
	// 	processManager.disableInterrupts(); 	//Disable interrupts, because addTask can also be called by interrupts.
	// 	TimedTask * taskToRemove = firstTask_toRemoveList;
	// 	firstTask_toRemoveList = nullptr;
	// 	processManager.enableInterrupts();
		
	// 	while (taskToRemove != nullptr) {
	// 		if (taskToRemove == firstTask) {
	// 			firstTask = firstTask->next;
	// 		} else {
	// 			TimedTask * current = firstTask;
	// 			while (current->next != nullptr) {
	// 				if (current->next == taskToRemove) {
	// 					current->next = taskToRemove->next;
	// 					taskToRemove->next = nullptr;
	// 				}
	// 			}
	// 		}
	// 		//check next in list
	// 		taskToRemove = taskToRemove->next;
	// 	}
	// }

	TimeValue _getTimeSinceStart();

	void updateTaskListWithTimePassage(TimeValue timePassed) {
		TimedTask * current = firstTask;
		while (current != nullptr) {
			current->timeHasPassed(timePassed);
			current = current->next;
		}
	}

};
