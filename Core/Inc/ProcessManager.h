#pragma once

#include <functional>

class ProcessRequest {
private:
	ProcessRequest * next = nullptr;
	std::function<void(void)> processFunction;
public:
	void execute() {
		processFunction();
	}
};

template<typename Platform>
class ProcessManager {
private:
	ProcessRequest * first = nullptr;
	ProcessRequest * last = nullptr;

public:
	void requestProcess(ProcessRequest * requester) {
		if ((requester->next != nullptr) || (first == requester) || (last == requester)) {
			//already present in queue somewhere
			return;
		}
		disableInterrupts();
		if (first == nullptr) {
			//nothing in queue, add it
			first = requester;
			last = requester;
		} else {
			//1 item in queue
			_addAfter(requester, last);
			last = requester;
		}
		enableInterrupts();
	}

	void execute() {
		disableInterrupts();
		while (first != nullptr) {
			first->execute();
			_removeFirst();
		}
		enableInterrupts();
	}

private:
	void disableInterrupts();
	void enableInterrupts();
	void _addAfter(ProcessRequest * toAdd, ProcessRequest * afterThis) {
		//assumes interrupts are already disabled
		toAdd->next = afterThis->next;
		afterThis->next = toAdd;
	}

	void _removeFirst() {
		//assumes interrupts are already disabled

		ProcessRequest * oldFirst = first;
		//set new first
		first = oldFirst->next; 	//set to nullptr if there's no next
		//reset old first
		oldFirst->next = nullptr;
	}

};


