#pragma once

#include "ProcessRequest.h"

template<typename Platform>
class ProcessManager {
private:
	ProcessRequest * first = nullptr;
	ProcessRequest * last = nullptr;

	bool wakeup = false;

public:
	void requestProcess(ProcessRequest & request) {
		if ((request.next != nullptr) || (first == &request) || (last == &request)) {
			//already present in queue somewhere
			return;
		}
		disableInterrupts();
		if (first == nullptr) {
			//nothing in queue, add it
			first = &request;
			last = &request;
		} else {
			//1 item in queue
			_addAfter(request, *last);
			last = &request;
		}
		enableInterrupts();
		awake();
	}

	void execute() {
		disableInterrupts();
		while (first != nullptr) {
			first->execute();
			_removeFirst();
		}
		enableInterrupts();
	}

	void sleep() {
		//NOTE: Please make a platform specific low-power-mode replacement for your used platform
		while(!wakeup) {
			//do nothing
		}
		//wakeup = false needs to happen AFTER sleep. We are now awake and ready to do work; any process that needs us will get attention if they ask. There's a (big) chance at an unneccessary 'wakeup', but mweh, it'll go back to sleep right away again. To (mostly) avoid that, set wakeup=false at the very start of TimingManager<Stm32F407Platform>::waitTillNextTask() (before the task-list update)
		wakeup = false;

	}
	void awake() {
		//NOTE: Please make a platform specific low-power-mode replacement for your used platform
		wakeup = true;
	}

	void disableInterrupts();
	void enableInterrupts();
public:
	void _addAfter(ProcessRequest & toAdd, ProcessRequest & afterThis) {
		//assumes interrupts are already disabled
		toAdd.next = afterThis.next;
		afterThis.next = &toAdd;
	}

	void _removeFirst() {
		//assumes interrupts are already disabled

		ProcessRequest * oldFirst = first;
		//set new first
		first = oldFirst->next; 	//set to nullptr if there's no next
		//reset old first
		oldFirst->next = nullptr;
		//reset last if it was also first (remove as well)
		if (last == oldFirst) {
			last = nullptr;
		}
	}

};


