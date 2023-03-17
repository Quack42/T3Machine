/*
 * MyExternalInterruptPin.h
 *
 *  Created on: Feb 20, 2023
 *  Author: quack
 */
#pragma once

#include "ProcessManager.h"

#include "PinIdentifier.h"

template<typename Platform>
class ExternalInterruptPin {
private:
	ProcessManager<Platform> & processManager;
	PinIdentifier<Platform> & pinIdentifier;
	bool flaggedInterrupt = false;
	bool activeLow = false;

	SubscriptionNode * firstSubscription;
	ProcessRequest advertiseProcess;

public:
	ExternalInterruptPin(ProcessManager<Platform> & processManager, PinIdentifier<Platform> & pinIdentifier, const bool & activeLow) :
			processManager(processManager),
			pinIdentifier(pinIdentifier),
			activeLow(activeLow),
			advertiseProcess(std::bind(&ExternalInterruptPin::advertise, this))
	{

	}

	void registerSubscription(SubscriptionNode & subscription) {
		if (firstSubscription == nullptr) {
			firstSubscription = &subscription;
		} else {
			SubscriptionNode * current = firstSubscription;
			while (current->next != nullptr) {
				current = current->next;
			}
			current->next = &subscription;
		}
	}
	//TODO: void removeSubscription(SubscriptionNode * subscription) ..

	void init();
	bool getValue();

	void flagInterrupt() {
		flaggedInterrupt = true;
		processManager.requestProcess(advertiseProcess);
	}

	bool isInterruptFlagged() {
		bool ret = flaggedInterrupt;
		//TODO: if an interrupt occurs here it can be missed. (flaggedInterrupt has already been read; <interrupt> flaggedInterrupt is set to true in flagInterrupt(); </interrupt> flaggedInterrupt is set back to false)
		flaggedInterrupt = false;
		return ret;
	}

	PinIdentifier<Platform> & getPinIdentifier() {
		return pinIdentifier;
	}

private:
	void advertise() {
		SubscriptionNode * current = firstSubscription;
		while (current != nullptr) {
			processManager.requestProcess(current->getProcessRequest());
		}
	}
};

#ifdef MOCK
	#include "MockExternalInterruptPin.h"
#endif //defined(MOCK)
