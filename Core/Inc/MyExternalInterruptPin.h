/*
 * MyExternalInterruptPin.h
 *
 *  Created on: Feb 20, 2023
 *  Author: quack
 */
#pragma once

#include "ProcessManager.h"

#include "PinIdentifier.h"


class ExternalInterruptPinSubscription {
private:
	bool pinValue;
	std::function<void(bool)> subscribedFunction;
};

template<typename Platform>
class ExternalInterruptPin {
private:
	ProcessManager<Platform> & processManager;
	PinIdentifier<Platform> & pinIdentifier;

	bool interruptValue = false;

	ProcessRequest advertiseProcess;

	std::function<void(bool)> subscriberFunction;

public:
	ExternalInterruptPin(ProcessManager<Platform> & processManager, PinIdentifier<Platform> & pinIdentifier) :
			processManager(processManager),
			pinIdentifier(pinIdentifier),
			advertiseProcess(std::bind(&ExternalInterruptPin::advertise, this))
	{

	}

	void init() {
		_init();
		interruptValue = _getValue();
	}

	bool getValue() {
		return interruptValue;
	}

	void setSubscriberFunction(std::function<void(bool)> subscriberFunction) {
		this->subscriberFunction = subscriberFunction;
	}

	void isr() {
		interruptValue = _getValue();
		processManager.requestProcess(advertiseProcess);
	}

	PinIdentifier<Platform> & getPinIdentifier() {
		return pinIdentifier;
	}

private:
	void advertise() {
		if (subscriberFunction) {
			subscriberFunction(interruptValue);
			// subscriberFunction(getValue()); 	//TODO: think about if this is the same as intValue; and if we can remove intValue
		}
	}

	void _init();
	bool _getValue();
};
