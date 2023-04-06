#pragma once

#include "TimingManager.h"
#include "ProcessManager.h"
#include "SubscriberLL.h"

// #include "MyGPIO.h" 	//TODO: REMOVE THIS
// #include "PlatformSelection.h" 	//TODO: REMOVE THIS
// extern OutputPin<Platform> ld6; 	//blue 	//TODO: REMOVE THIS
// extern OutputPin<Platform> ld5; 	//red 	//TODO: REMOVE THIS
// extern OutputPin<Platform> ld4; 	//grn 	//TODO: REMOVE THIS
// extern OutputPin<Platform> ld3; 	//ora 	//TODO: REMOVE THIS

template<typename Driver, typename Platform>
class SteppingTask {
private:
	// Constants
	constexpr static float kDefaultStepHighTime = 0.5f;
	constexpr static float kDefaultStepLowTime = 0.5f;

	// References
	ProcessManager<Platform> & processManager;
	TimingManager<Platform> & timingManager;
	Driver & driver;
	int & positionIndex;

	// Components
	TimedTask steppingControlTimedTask;

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
	SteppingTask(	ProcessManager<Platform> & processManager,
					TimingManager<Platform> & timingManager,
					Driver & driver,
					int & positionIndex,
					float stepHighTime = kDefaultStepHighTime,
					float stepLowTime = kDefaultStepLowTime) :
			// References
			processManager(processManager),
			timingManager(timingManager),
			driver(driver),
			positionIndex(positionIndex),
			// Components
			steppingControlTimedTask(std::bind(&SteppingTask<Driver, Platform>::steppingControl, this), TimeValue(stepHighTime)),
			// Variables
			stepHighTime(stepHighTime),
			stepLowTime(stepLowTime),
			steps(0),
			state(e_idle),
			stopping(false)
	{

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
			//assumes we checked the task was actually done stepping before we called this.
			return;
		}

		// ld4.high();

		//set variables for stepping task
		this->steps = steps;
		stopping = false;

		//set first step
		initiateStepping();
	}

	void stop() {
		stopping = true;
	}

	void steppingControl() {
		switch(state) {
			case e_idle:
				//do nothing
				break;
			case e_toIdle:
				state = e_idle;
				// // ld6.high();
				// ld5.low();
				// // ld4.low();
				advertiseStop();
				// ld6.low();
				break;
			case e_toInitiate:
				initiateStepping();
				break;
			case e_toStepHigh:
				stepHigh();
				break;
			case e_toStepLow:
				stepLow();
				break;
			default:
				//do nothing
				break;
		}
	}

private:
	void initiateStepping() {
		//set driver to step to appropriate direction
		bool directionHigh = (steps > 0);
		driver.setDirectionPin(directionHigh);
		//step
		stepHigh();
	}

	//task functions
	void stepHigh() {
		//set pin high
		// ld5.low();
		// ld3.high();
		driver.setStepPin(true);
		//indicate next state-step is to make it low
		state = e_toStepLow;
		//schedule end of high-phase
		steppingControlTimedTask.setTimeUntilTaskStart(TimeValue(stepHighTime));
		timingManager.addTask(&steppingControlTimedTask);
	}

	void stepLow() {
		//make pin low
		driver.setStepPin(false);

		//reduce number of steps to take
		if (steps > 0) {
			steps--;
		} else if (steps < 0) {
			steps++;
		}
		//check if steps are done
		if (steps == 0 || stopping) {
			//steps are done
			state = e_toIdle;
		} else {
			//need another step
			state = e_toStepHigh;
		}
		// ld3.low();
		// ld5.high();

		//update position index
		positionIndex++;
		//schedule end of low-phase
		steppingControlTimedTask.setTimeUntilTaskStart(TimeValue(stepLowTime));
		timingManager.addTask(&steppingControlTimedTask);
	}

	void advertiseStop() {
		/// Advertise
		while (stoppedSubscriberList != nullptr) {
			//first remove first from list
			Subscriber * current = stoppedSubscriberList;
			stoppedSubscriberList = stoppedSubscriberList->getNext();
			//then add to process managers to-call list.
			processManager.requestProcess(current->getProcessRequest());
		}
	}
};
