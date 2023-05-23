#pragma once

#include "ProcessManager.h"
#include "TimingManager.h"
#include "Timer.h"
#include "ProcessRequest.h"
#include "SubscriberLL.h"

#include "logging.h"

#include <functional>


template<typename Driver, typename Platform>
class SteppingTask {
private:
	// Static Constants
	constexpr static float kDefaultStepHighTime = 1.5f;
	constexpr static float kDefaultStepLowTime = 1.5f;
	
	// Member Constants
	const char identifier;

	// References
	ProcessManager<Platform> & processManager;
	TimingManager<Platform> & timingManager;
	Timer<Platform> & timer;
	Driver & driver;
	int & positionIndex;

	// Components
	ProcessRequest advertiseStopProcessRequest;

	// Variables
	float stepHighTime;
	float stepLowTime;
	int steps;
	enum {
		e_idle,
		e_toIdle,
		e_toInitiate,
		e_toStepHigh,
		e_toStepLow
	} state = e_idle;
	bool stopping;

	Subscriber * stoppedSubscriberList = nullptr;

public:
	SteppingTask(	const char identifier,
					ProcessManager<Platform> & processManager,
					TimingManager<Platform> & timingManager,
					Timer<Platform> & timer,
					Driver & driver,
					int & positionIndex,
					float stepHighTime = kDefaultStepHighTime,
					float stepLowTime = kDefaultStepLowTime) :
			// Constants
			identifier(identifier),
			// References
			processManager(processManager),
			timingManager(timingManager),
			timer(timer),
			driver(driver),
			positionIndex(positionIndex),
			// Components
			advertiseStopProcessRequest(std::bind(&SteppingTask<Driver, Platform>::advertiseStop, this)),
			// Variables
			stepHighTime(stepHighTime),
			stepLowTime(stepLowTime),
			steps(0),
			state(e_idle),
			stopping(false)
	{

	}

	void init() {
		timer.setISRCallbackFunction(std::bind(&SteppingTask<Driver, Platform>::timerISR, this));
	}

	void subscribeToStop(Subscriber * sub) {
		Subscriber::addSubscription(sub, &stoppedSubscriberList);
	}

	void setStepHighTime(const float & stepHighTime) {
		this->stepHighTime = stepHighTime;
	}

	void setStepLowTime(const float & stepLowTime) {
		this->stepLowTime = stepLowTime;
	}

	bool isActive() {
		return (state != e_idle);
	}

	void startSteppingTask(int steps) {
		if (state != e_idle) {
			// Assumes we checked the task was actually done stepping before we called this.
			return;
		}

		char buffer[] = "Step[*] -> start\n";
		buffer[5] = identifier;
		debug(buffer, sizeof("Step[*] -> start\n")-1);

		// Set variables for stepping task.
		if (steps != 0) {
			this->steps = steps;
			stopping = false;

			// Set first step.
			initiateStepping();
		} else {
			processManager.requestProcess(advertiseStopProcessRequest);
		}
	}

	void stop() {
		stopping = true;
	}

private:
	void timerISR() {
		if (state == e_toStepHigh) {
			stepHighISR();
		} else if(state == e_toStepLow) {
			stepLowISR();
		} else if(state == e_toIdle) {
			state = e_idle;
			processManager.requestProcess(advertiseStopProcessRequest);
		}
	}

	void initiateStepping() {
		// Set driver to step to appropriate direction
		bool directionHigh = (steps > 0);
		driver.setDirectionPin(directionHigh);
		// Start stepping
		state = e_toStepHigh;
		timer.setTime(TimeValue(stepLowTime));
		timer.start();
	}

	// Task functions.

	void stepHighISR() {
		// Set pin high.
		driver.setStepPin(true);
		// Indicate next state-step is to make it low.
		state = e_toStepLow;
		// Schedule end of high-phase.
		timer.setTime(TimeValue(stepHighTime));
		timer.start();
	}

	void stepLowISR() {
		// Make pin low.
		driver.setStepPin(false);

		// Reduce number of steps to take and update position index.
		if (steps > 0) {
			steps--;
			positionIndex++;
		} else if (steps < 0) {
			steps++;
			positionIndex--;
		}

		// Check if steps are done.
		if (steps == 0 || stopping) {
			// Steps are done.
			state = e_toIdle;
		} else {
			// Need another step.
			state = e_toStepHigh;
		}

		// Schedule end of low-phase.
		timer.setTime(TimeValue(stepLowTime));
		timer.start();
	}

	void advertiseStop() {
		char buffer[] = "Step[*] -> stop\n";
		buffer[5] = identifier;
		debug(buffer, sizeof("Step[*] -> stop\n")-1);

		/// Advertise.
		while (stoppedSubscriberList != nullptr) {
			// First remove first from list.
			Subscriber * current = stoppedSubscriberList;
			stoppedSubscriberList = stoppedSubscriberList->getNext();
			// Then add to process managers to-call list.
			processManager.requestProcess(current->getProcessRequest());
		}
	}
};
