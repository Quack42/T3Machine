#pragma once

#include <functional>

class ProcessRequest {
public:
	ProcessRequest * next = nullptr; 	//member variable as public.. ugh.. //TODO: fixme
private:
	std::function<void(void)> processFunction;
public:
	ProcessRequest(std::function<void(void)> processFunction) :
			processFunction(processFunction)
	{

	}

	void execute() {
		processFunction();
	}
};

class SubscriptionNode {
private:
	SubscriptionNode * next = nullptr;
	ProcessRequest processRequest;
public:
	SubscriptionNode(std::function<void(void)> processFunction) :
		processRequest(processFunction)
	{
	}

	ProcessRequest & getProcessRequest() {
		return processRequest;
	}
};

template<typename Platform>
class ProcessManager {
private:
	ProcessRequest * first = nullptr;
	ProcessRequest * last = nullptr;

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
	}

};


