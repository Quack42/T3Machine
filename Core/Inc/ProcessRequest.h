#pragma once

#include <functional>

class ProcessRequest {
public:
	ProcessRequest * next = nullptr; 	//member variable as public.. ugh.. //TODO: fixme
private:
	std::function<void(void)> processFunction;
public:
	ProcessRequest(std::function<void(void)> processFunction) :
			next(nullptr),
			processFunction(processFunction)
	{

	}

	void execute() {
		processFunction();
	}
};
